#! /usr/bin/python

__author__ = "sven"
__date__ = "$27-jan-2017 16:17:17$"

import time
import pycurl
from StringIO import StringIO
import MySQLdb as mdb # apt-get install python-mysqldb
import sys
import logging
from pprint import pprint


# Add column to database table
# mysql -u root -p
# use slide
# ALTER TABLE sliders ADD synced TINYINT(4) DEFAULT 0 NOT NULL;
# DESCRIBE sliders;
# exit

def uploadData(sHost, datetime, showselected, red, green, blue, white, score, duration, speed, total, photofile, rfid, slideid):
	c = pycurl.Curl()
	buffer = StringIO()

	postdata = [
		("datetime", str(datetime)),
		("showselected", str(showselected)),
		("red", str(red)),
		("green", str(green)),
		("blue", str(blue)),
		("white", str(white)),
		("score", str(score)),
		("duration", str(duration)),
		("speed", str(speed)),
		("total", str(total)),
		("photoname", str(photofile)),
		("rfid", str(rfid)),
		("slideid", str(slideid)),
#		("photo", (c.FORM_FILE, photofile)),
	]
	
	c.setopt(c.URL, "http://" + sHost + "/stats/backend/addslider.php")
#	c.setopt(c.HTTPHEADER, ["Accept:"]) #curl_setopt($curl,CURLOPT_HTTPHEADER, array("Expect:  "));
	c.setopt(c.HTTPHEADER, ["Expect:"])
#	c.setopt(c.WRITEDATA, buffer)
	c.setopt(c.WRITEFUNCTION, buffer.write)
	c.setopt(c.HTTPPOST, postdata)
#	c.setopt(pycurl.VERBOSE, 1) # enable for output
#	c.setopt(pycurl.DEBUGFUNCTION, test)
	c.perform()
	c.close()

	body = buffer.getvalue()
	print(body)
	return







def insertSql(query):
	try:
		conmdb = mdb.connect('127.0.0.1', 'web', 'w3b', 'slide')
		with conmdb:
			curmdb = conmdb.cursor()
			curmdb.execute(query)
			return curmdb.lastrowid
	except mdb.Error, e:
		logging.error("@libmdb.py/insertSql(%s): [%d] %s", query, e.args[0], e.args[1])
		sys.exit(1)
	finally:
		if conmdb:
			conmdb.close()

def updateSql(query):
	try:
		conmdb = mdb.connect('127.0.0.1', 'web', 'w3b', 'slide')
		with conmdb:
			curmdb = conmdb.cursor()
			curmdb.execute(query)
			return curmdb.rowcount
	except mdb.Error, e:
		logging.error("@libmdb.py/updateSql(%s): [%d] %s", query, e.args[0], e.args[1])
		sys.exit(1)
	finally:
		if conmdb:
			conmdb.close()
	
	
def selectSqlAll(query):
	try:
		conmdb = mdb.connect('127.0.0.1', 'web', 'w3b', 'slide')
		with conmdb:
			curmdb = conmdb.cursor(mdb.cursors.DictCursor)
			curmdb.execute(query)
			rows = curmdb.fetchall()
#			logging.debug("@libmdb.py/selectSqlAll(%s): %s", query, rows)
			return rows
	except mdb.Error, e:
		logging.error("@libmdb.py/selectSqlAll(%s): [%d] %s", query, e.args[0], e.args[1])
		sys.exit(1)
	finally:
		if conmdb:
			conmdb.close()


def selectSqlOne(query, field):
	try:
		conmdb = mdb.connect('127.0.0.1', 'web', 'w3b', 'slide')
		with conmdb:
			curmdb = conmdb.cursor(mdb.cursors.DictCursor)
			curmdb.execute(query)
			rowmdb = curmdb.fetchone()
#			logging.debug("@libmdb.py/selectSqlOne(%s, %d): %s", query, field, rowmdb)
			if (field == 0):
				return rowmdb
			else:
				if ((rowmdb != None) and (rowmdb[field - 1] != None)):
					return rowmdb[field - 1]
				else:
					return 0
	except mdb.Error, e:
		logging.error("@libmdb.py/selectSqlOne(%s, %s): [%d] %s", query, field, e.args[0], e.args[1])
		sys.exit(1)
	finally:
		if conmdb:
			conmdb.close()


def deleteSql(query):
	try:
		conmdb = mdb.connect('127.0.0.1', 'web', 'w3b', 'slide')
		with conmdb:
			curmdb = conmdb.cursor()
			curmdb.execute(query)
			return curmdb.rowcount
	except mdb.Error, e:
		logging.error("@libmdb.py/deleteSql(%s): [%d] %s", query, e.args[0], e.args[1])
		sys.exit(1)
	finally:
		if conmdb:
			conmdb.close()



def selectSlidersNoSynced(limit):
	ret = selectSqlAll("SELECT * FROM sliders WHERE synced = 0 ORDER BY date DESC LIMIT " + str(limit) + ";")
	return ret

def updateSliderSynced(id):
	ret = updateSql("UPDATE sliders SET synced = 1 WHERE id = " + str(id) + ";")
	return ret







if __name__ == "__main__":
	slideid = 127
	slidesync = 1000
	
	print "Running Sync task for " + str(slidesync) + " records...";
	sliders = selectSlidersNoSynced(slidesync)
	for slider in sliders:
#		pprint(slider)
		uploadData("global.ibgnetworx.nl", slider['date'], slider['showselected'], slider['red'], slider['green'], slider['blue'], slider['white'], slider['score'], slider['duration'], slider['speed'], slider['total'], slider['photo'], slider['rfid'], slideid)
		updateSliderSynced(slider['id'])
	
	
    