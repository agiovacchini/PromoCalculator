require 'rubygems'
require "dbi"

class Db

   @dbConn = 0
   @dbType = 0
   @dbInitialized = false
   
   def initialize()
    puts "oooo1 #{DB_URL}, #{DB_USER}, #{DB_PASSWORD}"
   	if !@dbInitialized
 	     	@dbConn = DBI.connect("#{DB_URL}", "#{DB_USER}", "#{DB_PASSWORD}")
 	     	@dbType = "#{DB_TYPE}"
 	     	puts self.class.to_s << " - DB Connection initialized"
 	     	@dbInitialized = true
 	   else
 	   	puts self.class.to_s << " - DB Connection already initialized"
     end
     puts "oooo2"
   end
   
   def getConn()
   	if @dbInitialized
   		return @dbConn
   	else 
   		return false
   	end
   end
   
   def close()
   	@dbConn.close
      @dbInitialized = true
   end
   
end
