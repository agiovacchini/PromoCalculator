require 'rubygems'
require 'savon'
require 'watir-webdriver'
require '../kStore/KStore.rb'
require 'json'
require 'net/ssh'

puts "Inizializzo kStoreServlet su url: #{KSTORE_SERVLET_URL}"
kStoreServlet = KStore.new("#{KSTORE_SERVLET_URL}", "JSON")
retCode = 0
posForSuspend = 0
cardCode = ""
myRequestId = "_"
result = Hash.new
tmpSession = ""
tmpReqId = ""
tmpReqNum = ""
tmpDevId = ""

Given(/^request "(.*?)" to initialize a new session with device "(.*?)"$/) do |arg1, arg2|
  myRequestId = arg1
  retCode = kStoreServlet.InitializeSession( myRequestId, arg2 )
end

Given(/^existing request "(.*?)"$/) do |arg1|
  myRequestId = arg1
end

Then(/^kStore should return RC "(.*?)" and a session id$/) do |arg1|
  sessionId = kStoreServlet.getSessionId(myRequestId).to_i
  retCode.should == arg1.to_i
  sessionId.should > 0
  puts "sessionId"
  puts sessionId.to_s
end


When(/^I add kStore a customer with code "(.*?)"$/) do |arg1|
  retCode = kStoreServlet.AddCustomer( myRequestId, arg1 )
end

Then(/^kStore should return RC "(.*?)" and the same card which has been sent$/) do |arg1|
  retCode.should == arg1.to_i
  puts "customerID"
  puts kStoreServlet.getCustomerId
end

When(/^I add kStore an item of type "(.*?)" with code "(.*?)"$/) do |arg1, arg2|
  retCode = kStoreServlet.AddItem( myRequestId, arg1, arg2, false )
  result = kStoreServlet.getResult
end

When(/^I void kStore an item of type "(.*?)" with code "(.*?)"$/) do |arg1, arg2|
  retCode = kStoreServlet.AddItem( myRequestId, arg1, arg2, true )
  result = kStoreServlet.getResult
end

When(/^I suspend kStore transaction for POS "(.*?)"$/) do |arg1|
  posForSuspend = arg1
  retCode = kStoreServlet.SuspendTransaction( myRequestId, posForSuspend )
end

When(/^I void kStore transaction$/) do
  retCode = kStoreServlet.VoidTransaction( myRequestId )
end

Then(/^kStore should return RC "(.*?)"$/) do |arg1|
  retCode.should == arg1.to_i
end

Then(/^kStore should return promotional information with a discount amount of "(.*?)" and "(.*?)" description$/) do |arg1, arg2|
  result["promoResponse"]["status"].to_i.should == 0
  result["promoResponse"]["promoValue"].to_f.should == arg1.to_f
  result["promoResponse"]["promoDescr"].to_s.should == arg2.to_s
end

Then(/^kStore should return no promotional information"$/) do
  result["promoResponse"]["status"].to_i.should == 4
end

Then(/^I close transaction on POS$/) do
  sleep 5
  Net::SSH.start('192.168.1.111', 'vstore', :password => "ciccio") do |ssh|
    output = ssh.exec "./liscrctl /T#{posForSuspend} /O2 /N999 /X1 /R1 /P /F"
  end
end

Given(/^existing session "(.*?)" with request id "(.*?)" and last request number "(.*?)" and device "(.*?)"$/) do |arg1, arg2, arg3, arg4|
  tmpSession = arg1
  tmpReqNum = arg3.to_i
  tmpDevId = arg4
  myRequestId = arg2
end

When(/^I recover session$/) do
  kStoreServlet.RecoverSession tmpSession, myRequestId, tmpReqNum, tmpDevId
end

When(/^I ask for totals$/) do
  retCode = kStoreServlet.GetTotals( myRequestId )
  puts kStoreServlet.totalAmount.to_s
end
