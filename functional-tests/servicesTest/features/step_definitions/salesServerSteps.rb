require 'rubygems'
require 'savon'
require 'json'
require 'net/ssh'
require '../servicesTest/salesServer/salesServer.rb'

retCode = 0

puts "Inizializzo sales server su: #{SALES_IP}:{SALES_PORT}"
salesServer = SalesServer.new()

sleepTime = 0.2

Given(/^request to initialize a new session$/) do
  retCode = salesServer.InitializeSession( "#{SALES_IP}", "#{SALES_PORT}" )
  sleep sleepTime
end

Given(/^request to open a new cart$/) do
  retCode = salesServer.OpenCart
  #puts retCode
  sleep sleepTime
end

Then(/^salesServer should return RC "(.*?)" and a cart id$/) do |arg1|
  puts "a"
end

Given(/^request to add "(.*?)" quantity of item "(.*?)" to cart "(.*?)"$/) do |arg1, arg2, arg3|
  retCode = salesServer.AddItem arg1, arg2, arg3
  sleep sleepTime
end

Given(/^request to remove item "(.*?)" from cart "(.*?)"$/) do |arg1, arg2|
  retCode = salesServer.RemoveItem arg1, arg2
  sleep sleepTime
end

Given(/^request to persist cart "(.*?)"$/) do |arg1|
  retCodr = salesServer.PersistCart arg1
end

Given(/^request to close connection$/) do
   retCode = salesServer.CloseSocket
end

