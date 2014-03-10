require '../common/db/Db.rb'

require File.dirname(__FILE__) + '/lib/configuration';

DB_URL = Configuration["dbUrl"]
DB_USER = Configuration["dbUser"]
DB_PASSWORD = Configuration["dbPassword"]
DB_TYPE = Configuration["dbType"]

puts 'dbUrl: ' << DB_URL
puts 'dbUser: ' << DB_USER
puts 'dbPassword: ' << DB_PASSWORD
puts 'dbType: ' << DB_TYPE

kloyDbInstance = Db.new
DB_CONN = kloyDbInstance.getConn()
