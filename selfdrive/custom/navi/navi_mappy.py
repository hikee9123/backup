import socket
import time

import threading
import select
import json
import cereal.messaging as messaging

from threading import Thread

class Port:
  BROADCAST_PORT = 2899
  RECEIVE_PORT = 2843


class MappyServer:
    def __init__(self):
        self.active = 0
        self.remote_addr = None
        self.last_exception = None

        self.speedLimit = 0
        self.speedLimitDistance = 0
        self.mapValid = 0
        self.trafficType  = 0
        self.safetySign1 = 0
        self.turnInfo = 0
        self.distanceToTurn = 0
        self.ts = 0
        self.idx = 0
        self.idx_old = 0

        self.dHideTimeSec = 0
        self.dArrivalTimeSec =  0
        self.dArrivalDistance =  0
        self.dEventSec = 0
        self.current_time_seconds = 0        

        self.pm = messaging.PubMaster(['naviCustom']) 
        self.sm = messaging.SubMaster(['carState'])    
        self.lock = threading.Lock()

        broadcast = Thread(target=self.udp_server_find_remote_ip, args=[])
        broadcast.daemon = True
        broadcast.start()


    def udp_server_find_remote_ip(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('0.0.0.0', Port.BROADCAST_PORT)  # 서버 주소와 포트 번호
        server_socket.bind( server_address )

        host_name = socket.gethostname()
        ip_address = socket.gethostbyname(host_name)
        print(f"host name: {host_name}, IP address: {ip_address}")
        print('UDP Server is listening on {}:{}'.format(*server_address))
        while True:
            data, remote_addr = server_socket.recvfrom(1024)
            print('Received message from {}: {}'.format( remote_addr, data.decode()))
            server_socket.sendto( f'echo_{host_name}'.encode(), remote_addr )                



  
    def get_value(self, key):
        value = 0
        try:
            if key == 'True':
                value = 1
            elif key == 'False':
                value = 0
            else:
                value = float(key)
        except Exception as e:
            print(f"key error occurred: {e}")
    
        return value

    def udp_recv(self, sock):
        ret = False
        try:
            ready = select.select([sock], [], [], 1.)
            ret = bool(ready[0])
            if ret:
                data, self.remote_addr = sock.recvfrom(2048)
                json_obj = json.loads(data.decode())
                print(f"json={json_obj}")  
 

                if 'speedLimit' in json_obj:
                    self.speedLimit = self.get_value(json_obj["speedLimit"])

                if 'speedLimitDistance' in json_obj:
                    self.speedLimitDistance = self.get_value(json_obj["speedLimitDistance"])

                if 'mapValid' in json_obj:
                    self.mapValid = self.get_value(json_obj["mapValid"])

                if 'trafficType' in json_obj:
                    self.trafficType = self.get_value(json_obj["trafficType"])

                if 'safetySign1' in json_obj:
                    self.safetySign1 = self.get_value(json_obj["safetySign1"])
             
                if 'turnInfo' in json_obj:
                    self.turnInfo = self.get_value(json_obj["turnInfo"])

                if 'distanceToTurn' in json_obj:
                    self.distanceToTurn = self.get_value(json_obj["distanceToTurn"])

                if 'ts' in json_obj:
                    self.ts = self.get_value(json_obj["ts"])

                if 'id' in json_obj:
                    self.idx = self.get_value(json_obj["id"])                        

        except:
            try:
                self.lock.acquire()
            finally:
                self.lock.release()

        return ret
    
    def arrival_time( self, fDistance,  fSpeed_ms ):
        if fSpeed_ms:
            farrivalTime = fDistance / fSpeed_ms
        else:
            farrivalTime = fDistance
    
        return farrivalTime


    def update_event( self, speedms ):
        dEventDistance = self.speedLimitDistance

        if dEventDistance > 10:
            dArrivalSec = self.arrival_time( dEventDistance, speedms )

            self.dHideTimeSec = self.ts + dArrivalSec
            self.dArrivalTimeSec =  dArrivalSec
            self.dArrivalDistance =  dEventDistance
        else:
            self.dHideTimeSec =  self.ts + 5


        
    def update(self):
        self.sm.update(0)
        dSpeed_ms = self.sm["carState"].vEgo

        self.update_event( dSpeed_ms )
        dEventLastSec = self.ts - self.dEventSec 

        if dSpeed_ms > 2:
            dEventLastSec = self.current_time_seconds - self.dEventSec
            dArrivalTimeSec = self.dHideTimeSec - self.current_time_seconds
            dArrivalDistance =  dArrivalTimeSec * dSpeed_ms      
            if dSpeed_ms < 10:
                self.dEventHideSec = 20
            elif dSpeed_ms < 20:
                self.dEventHideSec = 10
            else:
                self.dEventHideSec = 7

            if dEventLastSec > self.dEventHideSec:
                self.speedLimit = 0
            elif dArrivalTimeSec < 1.5:
                self.speedLimit = 0
        else:
            self.dHideTimeSec =  self.current_time_seconds + 5

        if self.idx_old != self.idx:
            self.idx_old = self.idx
            self.dEventSec = self.ts

        dat = messaging.new_message('naviCustom')
        dat.naviCustom.naviData = {
            "active": self.mapValid,
            "roadLimitSpeed": 0,
            "isHighway": False,
            "camType": self.trafficType,
            "camLimitSpeedLeftDist": self.speedLimitDistance,
            "camLimitSpeed": self.speedLimit,
            "sectionLimitSpeed": 0,
            "sectionLeftDist": 0,
            "sectionAvgSpeed": 0,
            "sectionLeftTime": 0,
            "sectionAdjustSpeed": False,
            "camSpeedFactor": self.safetySign1,
            "currentRoadName": "",
            "isNda2": False,
            "cntIdx": self.idx,
        }
        self.pm.send('naviCustom', dat )



def main():

    server = MappyServer() 
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            sock.bind(('0.0.0.0', Port.RECEIVE_PORT))
            sock.setblocking(False)
            while True:
                if server.udp_recv(sock) and server.remote_addr:
                   server.update()
                #else:
                   #print(f'wait connect port={Port.RECEIVE_PORT}  remote_addr={server.remote_addr}')
                   #time.sleep( 0.5 )

                time.sleep( 0.5 )

        except Exception as e:
            print(f"An error occurred: {e}")
            server.last_exception = e                


if __name__ == "__main__":
  main()

