require 'rubygems'
require 'savon'
require 'json'
require 'net/ssh'
require '../servicesTest/salesServer/salesServer.rb'

respCartId = 0
respReqId = 0
respRc = 0

cartId = 0


puts "Inizializzo sales server su: #{SALES_IP}:{SALES_PORT}"
salesServer = SalesServer.new()

sleepTime = 0

Given(/^I wait forever$/) do
  while 0!=1
    sleep 0.1
  end
end

Given(/^request to initialize a new session$/) do
  result = salesServer.InitializeSession( "#{SALES_IP}", "#{SALES_PORT}" )
  puts result
  sleep sleepTime
end

Given(/^I use existing cart "(.*?)"$/) do |pCartId|
  cartId = pCartId
  puts "cartId: #{cartId}"
  sleep sleepTime
end

Given(/^request to open a new cart$/) do
  cartId = 0
  result =  salesServer.OpenCart
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  cartId = respCartId
  rc = result["rc"]
  puts "cartId: #{cartId}, reqId: #{respReqId}, rc: #{rc}"
  sleep sleepTime
end

Then(/^salesServer should return RC (\d+) and a cart id$/) do |expectedRc|
  respCartId.should > 0
  respRc.should == expectedRc.to_i
end

Then(/^salesServer should return RC (\d+) and my cart id$/) do |expectedRc|
  respCartId.should == cartId.to_i
  respRc.should == expectedRc.to_i
end

Given(/^request to add "(.*?)" quantity of item "(.*?)" to cart$/) do |arg1, arg2|
  result = salesServer.AddItem arg1, arg2, cartId
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  respRc = result["rc"]
  puts respRc
  sleep sleepTime
end

Given(/^request to remove item "(.*?)" from cart$/) do |arg1|
  result = salesServer.RemoveItem arg1, cartId
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  respRc = result["rc"]
  puts respRc
  sleep sleepTime
end

Given(/^request to print cart$/) do
  result = salesServer.PrintCart cartId
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  respRc = result["rc"]
  puts respRc
end

Given(/^request to persist cart$/) do
  result = salesServer.PersistCart cartId
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  respRc = result["rc"]
  puts respRc
end

Given(/^request to close connection$/) do
  result = salesServer.CloseSocket
  respCartId = result["cartId"]
  respReqId = result["reqId"]
  respRc = result["rc"]
  puts respRc
end

