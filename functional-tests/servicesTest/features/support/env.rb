require File.dirname(__FILE__) + '/lib/configuration';

AUTH_SERVICE_URL = Configuration["authServiceUrl"]
KWL_SERVLET_URL = Configuration["kwlServletUrl"]
KWL_UTIL_SERVLET_URL = Configuration["kwlUtilServletUrl"]
LOYALTY_WS_URL = Configuration["LoyaltyWsUrl"]
EKO_LOY_SERVLET_URL = Configuration["ekoLoyServletUrl"]
KSTORE_SERVLET_URL = Configuration["kStoreServletUrl"]
VS5_SERVER_IP = Configuration["Vs5ServerIp"]
VS5KLOYSVC_PORT= Configuration["Vs5KloySvcPort"]
TOSHIBA_MOBILE_HOST = "192.168.202.50"
TOSHIBA_MOBILE_PORT = "25000"
#E' SEMPRE LA MACHINA LOCALE
TOSHIBA_CASH_HOST = "127.0.0.1"
TOSHIBA_CASH_PORT = "6740"

DEBUG_STEPS = Configuration["debugSteps"]

puts 'authServiceUrl: ' << AUTH_SERVICE_URL
puts 'kwlServletUrl: ' << KWL_SERVLET_URL
puts 'kwlUtilServletUrl: ' << KWL_UTIL_SERVLET_URL
puts 'ekoLoyServletUrl: ' << EKO_LOY_SERVLET_URL
puts 'LoyaltyWsUrl: ' << LOYALTY_WS_URL

puts 'TOSHIBA_MOBILE_HOST: ' << TOSHIBA_MOBILE_HOST
puts 'TOSHIBA_MOBILE_PORT: ' << TOSHIBA_MOBILE_PORT
puts 'TOSHIBA_CASH_HOST: ' << TOSHIBA_CASH_HOST
puts 'TOSHIBA_MOBILE_PORT: ' << TOSHIBA_CASH_PORT
puts 'VS5_SERVER_IP: ' << VS5_SERVER_IP
puts 'VS5KLOYSVC_PORT: ' << VS5KLOYSVC_PORT

puts 'debugSteps: ' << DEBUG_STEPS.to_s
