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


class FindRemoteIP:
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

        self.pm = messaging.PubMaster(['naviCustom'])    
        self.lock = threading.Lock()

        broadcast = Thread(target=self.udp_server_find_remote_ip, args=[])
        broadcast.daemon = True
        broadcast.start()


    def udp_server_find_remote_ip(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server_address = ('0.0.0.0', Port.BROADCAST_PORT)  # 서버 주소와 포트 번호
        server_socket.bind( server_address )
  
        print('UDP Server is listening on {}:{}'.format(*server_address))
        while True:
            data, self.remote_addr = server_socket.recvfrom(1024)
            print('Received message from {}: {}'.format( self.remote_addr, data.decode()))
            server_socket.sendto( 'echo'.encode(), self.remote_addr )
  
    def get_value(self, key):
        value = 0
        try:
            value = json.dumps( key )
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
 
                if 'echo' in json_obj:
                    try:
                        echo = json.dumps(json_obj["echo"])
                        sock.sendto(echo.encode(), self.remote_addr ) # (self.remote_addr[0], Port.BROADCAST_PORT))
                    except:
                        pass

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
    
    def update(self):



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

    server = FindRemoteIP() 
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            sock.bind(('0.0.0.0', Port.RECEIVE_PORT))
            sock.setblocking(False)
            while True:
                if server.udp_recv(sock) and server.remote_addr:
                    server.update()

                time.sleep( 0.5 )

        except Exception as e:
            print(f"An error occurred: {e}")
            server.last_exception = e                


if __name__ == "__main__":
  main()

