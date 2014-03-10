require 'rubygems'
require './db/kloy/Loy/LoyQueryes.rb'
require 'dbi'
require 'CSV'

fileName  = " "
tableName = " "

loyQueryes = LoyQueryes.new
loyQueryes.initStatements

Given(/^a file named "(.*?)" containing all "(.*?)" data$/) do |arg1, arg2|
  fileName = arg1
  tableName = arg2
  #puts arg1.to_s << " " << arg2.to_s
end

Then(/^it should be loaded$/) do
  CSV.foreach(fileName, :headers => true) do |row|
    hashedRow = row.to_hash
    puts hashedRow



  end
end