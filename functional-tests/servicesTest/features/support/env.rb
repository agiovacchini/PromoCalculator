require File.dirname(__FILE__) + '/lib/configuration';

SALES_IP = Configuration["salesIp"]
SALES_PORT = Configuration["salesPort"]
puts 'salesIp: ' << SALES_IP
puts 'salesPort: ' << SALES_PORT
