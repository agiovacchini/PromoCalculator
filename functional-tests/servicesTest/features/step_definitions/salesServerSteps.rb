require 'rubygems'
require 'savon'
require 'json'
require 'net/ssh'
require '../servicesTest/salesServer/salesServer.rb'

retCode = 0
cartId = 0

puts "Inizializzo sales server su: #{SALES_IP}:{SALES_PORT}"
salesServer = SalesServer.new()

sleepTime = 0.2

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
  result = JSON.parse( salesServer.OpenCart )
  cartId = result["cartId"]
  puts "cartId: #{cartId}"
  sleep sleepTime
end

Then(/^salesServer should return RC "(.*?)" and a cart id$/) do |arg1|
  cartId.should > 0
end

Given(/^request to add "(.*?)" quantity of item "(.*?)" to cart$/) do |arg1, arg2|
  retCode = salesServer.AddItem arg1, arg2, cartId
  puts retCode
  sleep sleepTime
end

Given(/^request to remove item "(.*?)" from cart$/) do |arg1|
  retCode = salesServer.RemoveItem arg1, cartId
  puts retCode
  sleep sleepTime
end

Given(/^request to print cart$/) do
  retCode = salesServer.PrintCart cartId
  puts retCode
end

Given(/^request to persist cart$/) do
  retCode = salesServer.PersistCart cartId
  puts retCode
end

Given(/^request to close connection$/) do
   retCode = salesServer.CloseSocket
   puts retCode
end

