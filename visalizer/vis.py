import http.server
import socketserver
import urllib.parse

import subprocess
from urllib import request, parse

def getunt(f,s):
	res = b''
	while res[-len(s):] != s:
		c = f.read(1)
		res += c
		#print(res)
	return res

proc = subprocess.Popen(["../interact/interact"], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
getunt(proc.stdout,b']\n\n')
getunt(proc.stdout,b'\n')
getunt(proc.stdout,b'\n')
getunt(proc.stdout,b'\n')

PORT = 8001

center = None

def content():
	global center 
	#return open('vis.html','rb').read()
	with open('vis_template.html','r') as fp:
		base = fp.read()
	with open('plot.txt','r') as fp:
		data = fp.read().split('\n')
	
	def get_git(n,c):
		print(n,c)
		return """
		.grid-item%s {
				background-color: rgba(%s, 1);
		}
		""" % (str(ord(n)),c)
	gits = (
		get_git('.','0, 0, 0') + 
		get_git('+','0, 255, 255') +
		get_git('0','255, 255, 255') +
		get_git('1','128, 128, 128') +
		get_git('2','128, 0, 0') +
		get_git('3','0, 0, 128')
	)
	
	
	grid = ""
	for y,s in enumerate(data):
		for x,c in enumerate(s):
			grid += '<div class="grid-item{}" onclick="javascript:submitpos({},{})"></div>'.format(str(ord(c)),y,x)
			if c == '+':
				center = (y,x)
	
	html = base % (' 20px' * len(data[0]), ' 20px' * len(data), gits, grid)
	
	return html.encode('utf-8')

open('o.html','wb').write(content())



class Handler(http.server.SimpleHTTPRequestHandler):
	def do_GET(self):
		self.send_response(200)
		self.send_header('Content-type', 'text/html')
		self.end_headers()
		self.wfile.write(content())

	def do_POST(self):
		content_length = int(self.headers['Content-Length'])
		body = urllib.parse.unquote(self.rfile.read(content_length).decode('ascii'))
		print("post_body(%s)" % (body))
		cord = body.split('&')[0].split('=')
		assert(cord[0] == 'coordinate')
		cord = cord[1]
		if cord != 'None':
			cord = list(map(int,cord.split(',')))
			proc.stdin.write(b'%d %d\n' % (cord[1]-center[1],cord[0]-center[0]))
			proc.stdin.flush()
			while True:
				getunt(proc.stdout,b']\n\n')
				print('updated subprocess')
				s = getunt(proc.stdout,b'\n')
				hs = b'send this to aliens '
				if s[:len(hs)] == hs:
					s = s.split(b':')[1][1:-1]
					url = "https://icfpc2020-api.testkontur.ru/aliens/send?apiKey=7ffc9148909e45af9c34d5610604bc2c"
					res = request.urlopen(request.Request(url, data=s)).readline()
					proc.stdin.write(res + b'\n')
					proc.stdin.flush()
				else:
					break
		
		self.send_response(200)
		self.send_header('Content-type', 'text/html')
		self.end_headers()
		self.wfile.write(content())

httpd = socketserver.TCPServer(("", PORT), Handler)

print("serving at port", PORT)
httpd.serve_forever()
