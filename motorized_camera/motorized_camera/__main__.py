__author__ = 'gillesv'

import time
import BaseHTTPServer
import cgi
import socket
import argparse
from camera import CameraControl


camera=CameraControl(120,90,10)


class GeneratorHandler(BaseHTTPServer.BaseHTTPRequestHandler):
        def do_HEAD(self):
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()

        def do_GET(self):
            global camera
            print time.asctime(),self.path
            self.send_response(200)
            self.send_header("Content-type", "text/html")
            self.end_headers()
            if self.path.startswith("/movecamera"):
                movedir = self.path.split("/")[-1]
                try:
                    camera.move(movedir)
                    self.wfile.write("Camera OK")
                except:
                    print("camera error")
                    self.wfile.write("Camera error")
            else:
                htmlfile=__loader__.get_data("motorized_camera/remotectrl.html")
                self.wfile.write(htmlfile)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--usb", type=str,help="-USB port",default=None)
    parser.add_argument("--host", type=str, help="Host Name", default=socket.gethostname())
    parser.add_argument("--port", type=int, help="Port number", default=8000)
    args = parser.parse_args()


    if args.usb is not None:
        camera.usbport=args.usb

    server_class = BaseHTTPServer.HTTPServer
    httpd = server_class((args.host, args.port), GeneratorHandler)
    print time.asctime(), "Server Starts - %s:%s" % (args.host, args.port)
    try:
            httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    print time.asctime(), "Server Stops - %s:%s" % (args.host, args.port)
