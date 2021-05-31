import sys
import math
import socket
import threading
import _thread
import time
import argparse
import datetime
import random

null_player = 255

default_user_data = {
	"authorized": False,
	"id": -1,
	"name": "",
	"socket": None,
	"admin": False,
	"suspend_time": 0,
	"failed": False,
	"opened": 0,
	"mined": 0,
	"mines": 16,
	"bot": False
}

_print = print
def print(*args, **kwargs):
	ctime = datetime.datetime.now().strftime("[%H:%M:%S]: ")
	_print(ctime, sep='', end='')
	_print(*args, **kwargs)

def msg(data):
	return len(data).to_bytes(4, 'big') + data

class Cell:
	def __init__(self, player = 0):
		self.mined = False
		self.player = player
		self.flag = []

class Field:
	def generate_field_part(self, player, top, width, height):
		bottom = top+height
		if len(self.data) <= bottom:
			for i in range(bottom-len(self.data)):
				self.data += [[]]
		for y in range(top, bottom):
			for i in range(width):
				self.data[y] += [Cell(player)]

	def __init__(self, pcount, width, height):
		self.data = []
		hcount = int(math.sqrt(pcount))
		wcount = int(pcount/hcount)
		player = 0
		for y in range(hcount):
			for x in range(wcount):
				self.generate_field_part(player, y*height, width, height)
				player += 1
		y += 1
		while player < pcount:
			self.generate_field_part(player, y*height, width, height)
			player += 1

	def is_cell_exists(self, x, y):
		if x >= 0 and y >= 0:
			if y < len(self.data) and x < len(self.data[y]):
				return True
		return False

	def is_mined(self, x, y):
		if self.is_cell_exists(x, y):
			return self.data[y][x].mined
		return False

	def is_flagged(self, x, y, idp):
		if self.is_cell_exists(x, y):
			try:
				return idp in self.data[y][x].flag
			except:
				pass
		return False

	def get_cell_status(self, x, y, idp, own):
		if own:
			result = 1 + self.is_mined(x, y)*100
		else:
			result = self.is_flagged(x, y, idp)*10
		return result

	def get_mine_count(self, x, y):
		count = 0
		if self.is_cell_exists(x, y):
			count += self.is_mined(x-1, y)
			count += self.is_mined(x-1, y-1)
			count += self.is_mined(x, y-1)
			count += self.is_mined(x+1, y-1)
			count += self.is_mined(x+1, y)
			count += self.is_mined(x+1, y+1)
			count += self.is_mined(x, y+1)
			count += self.is_mined(x-1, y+1)
		return count

	# map structure
	# [4 byte row length] + row * [2 byte uid, 1 byte mine count, 1 byte mined & flag]
	def cell_map(self, idp):
		lst = b'\x01'
		for y in range(len(self.data)):
			lst += len(self.data[y]).to_bytes(4, 'big')
			for x in range(len(self.data[y])):
				lst += self.data[y][x].player.to_bytes(2, 'big')
				if self.data[y][x].player == idp:
					lst += self.get_mine_count(x, y).to_bytes(1, 'big')
					lst += self.get_cell_status(x, y, idp, True).to_bytes(1, 'big')
				else:
					lst += b'\x00'
					lst += self.get_cell_status(x, y, idp, False).to_bytes(1, 'big')
		lst += b'\x00'
		#print(lst)
		return lst

class Game:
	def authorize_user(self, client, player_name, token, user_data, bot = False):
		if (int(args.count) > len(self.players)) or (bot):
			print(player_name, "joined the game")
			user_data.update({"name": player_name})
			user_data.update({"authorized": True})
			user_data.update({"id": len(self.players)})
			user_data.update({"socket": client})
			user_data.update({"bot": bot})
			if token == args.key:
				user_data.update({"admin": True})
				print(player_name, "now controls the game")
			self.players += [user_data]
			output_data = b'OK\x00'
		else:
			output_data = b'ERROR\x00'
		return output_data

	def disconnect_user(self, uid):
		del self.players[uid]
		for i in range(uid, len(self.players)):
			self.players[i]["id"] -= 1

	def suspend_user(self, uid):
		suspend_time = 15
		self.players[uid].update({"suspend_time": time.time()+suspend_time})
		self.players[uid]['socket'].send(msg(b'\x02'+suspend_time.to_bytes(4, 'big')))

	def send_map(self):
		for player in self.players:
			if not (player['socket'] is None):
				player['socket'].send(msg(self.field.cell_map(player['id'])))

	def check_map(self):
		#get user list, who has no cells
		lst = self.players.copy()
		for y in range(len(self.field.data)):
			for x in range(len(self.field.data[y])):
				if not self.field.data[y][x].mined:
					uid = self.field.data[y][x].player
					lst = [i for i in lst if not (i['id'] == uid)]
		#send message about game fail
		for user in lst:
			if not user['failed']:
				user['failed'] = True
				user['socket'].send(msg(b'\x03'))
				print(user['name'], "has failed the game")
		#replace mined cells to free 
		lst_id = [i['id'] for i in lst]
		for y in range(len(self.field.data)):
			for x in range(len(self.field.data[y])):
				if self.field.data[y][x].player in lst_id:
					self.field.data[y][x].player = null_player
					self.field.data[y][x].mined = False
					self.field.data[y][x].flag = []
		#if there's only one player left in the game
		victorious = 1
		not_failed_users = 0
		for user in self.players:
			if user['failed'] == False:
				not_failed_users += 1
				victorious = user['id']
				if not_failed_users > 1:
					return
		for user in self.players:
			user['socket'].send(msg(b'\x04'+self.get_player_list()))

	def begin_game(self, user_data, bots, diff):
		if user_data['admin']:
			print('Starting game...')
			self.bot_count = bots
			for i in range(bots):
				self.bots += [Bot(self, "BOT"+str(i), diff)]
			self.field = Field(len(self.players), 8, 8)
			self.send_map()
			print('Game started')
		for player in self.players:
			self.send_mine_count(player['id'])
		return b''

	def get_player_list(self):
		players_bytes = b''
		players_bytes += len(self.players).to_bytes(4, 'big')
		for player in self.players:
			player_id = player['id'].to_bytes(4, 'big')
			player_failed = player['failed'].to_bytes(1, 'big')
			player_opened = player['opened'].to_bytes(4, 'big')
			player_mined = player['mined'].to_bytes(4, 'big')
			player_name = str.encode(player['name'])
			player_name_size = len(player_name).to_bytes(4, 'big')
			players_bytes += player_id+player_failed+player_opened+player_mined+player_name_size+player_name
		return players_bytes

	def send_mine_count(self, idp):
		self.players[idp]['socket'].send(msg(b'\x05'+self.players[idp]['mines'].to_bytes(4, 'big')))

	def left_click(self, idp, x, y):
		if self.field.data[y][x].player == idp:
			if self.players[idp]['mines'] > 0:
				self.players[idp]['mined'] += 1
				self.players[idp]['mines'] -= 1
				self.field.data[y][x].mined = True
				self.send_mine_count(idp)
		else:
			self.players[idp]['opened'] += 1
			self.field.data[y][x].player = idp
			if self.field.data[y][x].mined:
				self.field.data[y][x].mined = False
				self.field.data[y][x].flag = []
				self.suspend_user(idp)
			self.check_map()

	def right_click(self, idp, x, y):
		if self.field.data[y][x].player != idp:
			if idp in self.field.data[y][x].flag:
				self.field.data[y][x].flag.remove(idp)
			else:
				self.field.data[y][x].flag.append(idp)

	def user_input(self, user_data, click_type, cellx, celly):
		if self.field.is_cell_exists(cellx, celly):
			if click_type == 0:
				self.left_click(user_data["id"], cellx, celly)
			if click_type == 1:
				self.right_click(user_data["id"], cellx, celly)
			self.send_map()
		return b''

	def __process(self, client, address, read_data, user_data):
		if len(read_data) < 1:
			return b''
		print('MSG from:', address, ":", read_data)
		if user_data['suspend_time'] > 0 and time.time() < user_data['suspend_time']:
			return b''
		if user_data['failed']:
			return b''
		output_data = b''
		# Authorization
		if user_data['authorized'] == False:
			if read_data[:5] ==  b'\xc1\xd0\xdf\xd5\xe0':
				player_name_bytes = int.from_bytes(read_data[5:9], 'big')
				read_player = 9+player_name_bytes
				player_name = read_data[9:read_player].decode('utf-8')

				token_bytes = int.from_bytes(read_data[read_player:read_player+4], 'big')
				read_token = read_player+4+token_bytes
				token = read_data[read_player+4:read_token].decode('utf-8')
				return self.authorize_user(client, player_name, token, user_data)
		else:
			if read_data[0] == 1:
				bots = int.from_bytes(read_data[1:5], 'big')
				diff = int.from_bytes(read_data[5:9], 'big')
				return self.begin_game(user_data, bots, diff)
			if read_data[0] == 2:
				click_type = int.from_bytes(read_data[1:5], 'big')
				cellx = int.from_bytes(read_data[5:9], 'big')
				celly = int.from_bytes(read_data[9:14], 'big')
				return self.user_input(user_data, click_type, cellx, celly)
		return output_data

	def __slice(self, client, address, read_data, user_data):
		p = 0
		while (p < len(read_data)):
			pkg_size = int.from_bytes(read_data[p:p+4], 'little')
			output_data = self.__process(client, address, read_data[p+4:p+pkg_size+4], user_data)
			if len(output_data) > 0:
				client.send(msg(output_data))
			p += pkg_size+4

	def __read(self, client, address):
		# Communication with client's socket
		user_data = default_user_data.copy()
		while True:
			try:
				read_data = client.recv(4096) #read data
			except: #disconnected
				if user_data["authorized"] == True:
					print('Disconnected:', address)
					self.disconnect_user(user_data["id"])
				break
			self.__slice(client, address, read_data, user_data)
		if (len(self.players)-len(self.bots) == 0) and (args.autoshutdown): 
			print('Server closed')
			_thread.interrupt_main()

	def __accept(self):
		# Wait for a new connection
		while True:
			if self.allow_connection:
				# Accept connection
				client, address = self.sock.accept()
				print('New connection:',address)
				# Reading data
				t = threading.Thread(target=self.__read, args=(client, address))
				t.daemon = True
				t.start()
			else:
				time.sleep(3)

	def __timer(self):
		while True:
			player_list = self.get_player_list()
			for player in self.players:
				if not (player['socket'] is None):
					player['socket'].send(msg(b'\x00'+player_list))
			time.sleep(2)

	def open(self, host = '', port = 29965, pcount = 0, key = ''):
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		self.sock.bind((host, int(port)))
		self.sock.listen(5) # Max queue
		self.allow_connection = True
		print('Starting server ', host, ':', port, ' ...', sep='')
		t = threading.Thread(target=self.__accept)
		t.daemon = True
		t.start()
		t = threading.Thread(target=self.__timer)
		t.daemon = True
		t.start()
		print('Server started')

	def __init__(self):
		self.players = []
		self.bots = []

class BotSocket:
	def send(self, data):
		#print('[',self.bot.name,'] Received: ', data, sep='')
		if self.bot.user_data['failed'] or (len(data) > 4 and data[4] == 4):
			self.bot.stopped = True
			#self.bot.game.disconnect_user(self.bot.user_data['id'])
	
	def __init__(self, bot):
		self.bot = bot

class Bot:
	def __timer_set_mines(self):
		while not hasattr(self.game, 'field'):
			time.sleep(1)
		while (not self.stopped) and (self.user_data['mines'] > 0):
			#TODO: Сreate smarter algorithm
			#Continuous cycle isn't good practice
			y = random.randint(0, len(self.game.field.data)-1)
			x = random.randint(0, len(self.game.field.data[y])-1)
			if self.game.field.data[y][x].player == self.user_data['id']:
				self.game.left_click(self.user_data['id'], x, y)
				self.game.send_map()
				time.sleep(1)

	def __timer(self):
		while not hasattr(self.game, 'field'):
			time.sleep(1)
		while not self.stopped:
			time.sleep(random.uniform(0.1, 500/self.diff))
			if self.user_data['suspend_time'] > 0 and time.time() < self.user_data['suspend_time']:
				continue
			while not self.stopped:
				y = random.randint(0, len(self.game.field.data)-1)
				x = random.randint(0, len(self.game.field.data[y])-1)
				if self.game.field.data[y][x].player != self.user_data['id']:
					if self.game.field.data[y][x].mined:
						if random.uniform(0, self.diff) > 30:
							continue
					self.game.left_click(self.user_data['id'], x, y)
					self.game.send_map()
					break

	def calculate_diff(self, cdiff, bid, bots):
		diff = ((cdiff*100)+100)/(bid+1)
		if bid > 1:
			diff = (diff * 1.2) / bots
		return diff

	def __init__(self, game, name, diff):
		self.game = game
		self.name = name
		self.stopped = False
		self.diff = self.calculate_diff(diff, len(self.game.bots), self.game.bot_count)
		self.socket = BotSocket(self)
		self.user_data = default_user_data.copy()
		self.game.authorize_user(self.socket, self.name, '', self.user_data, True)
		print('New AI:',name, "Diff:",self.diff)
		t = threading.Thread(target=self.__timer)
		t.daemon = True
		t.start()
		t = threading.Thread(target=self.__timer_set_mines)
		t.daemon = True
		t.start()

#################################

parser = argparse.ArgumentParser(
    description='Minesweeper Battle Royale Server'
)
parser.add_argument('-s', '--server', metavar='server', required=True, help='server address')
parser.add_argument('-p', '--port', metavar='port', required=True, help='server port')
parser.add_argument('-c', '--count', metavar='count', required=True, help='number of online players')
parser.add_argument('-k', '--key', metavar='key', required=True, help='secret key')
parser.add_argument('--autoshutdown', metavar='autoshutdown', required=False, default=False, nargs='?', const=True, help='close the server when there are no players')
args = parser.parse_args()
print("Host property: ", args.server, sep='')
print("Port property: ", args.port, sep='')
print("Count property: ", args.count, sep='')
print("Key property: ", args.key, sep='')
print("Shutdown property: ", args.autoshutdown, sep='')
x = Game()
x.open(args.server, args.port, args.count, args.key)
while True:
	pass
	time.sleep(1)