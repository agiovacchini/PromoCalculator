require 'rubygems'
require 'httparty'
require 'socket'

class SalesServer

  @socket = nil
  @thrListen = nil
  Thread.abort_on_exception = true

  def InitializeSession( pIp, pPort )
    puts "ip: #{pIp}, port: #{pPort}"
    @socket = TCPSocket.open(pIp, pPort)
    #@thrListen = Thread.new {
    #  while 0!=1
    #    puts @socket.gets
    #  end
    #}
  end

  def OpenCart( )
    msg = "{\"action\":\"init\"}\n"
    puts "--- #{msg} ---"
    @socket.puts(msg)
  end

  def PrintCart( pCart )
    msg = "{\"action\":\"print\",\"cartId\":#{pCart}}\n"
    puts "--- #{msg} ---"
    @socket.puts(msg)
  end

  def PersistCart( pCart )
    msg = "{\"action\":\"save\",\"cartId\":#{pCart}}\n"
    puts "--- #{msg} ---"
    @socket.puts(msg)
  end

  def AddItem( pQty, pBarcode, pCart )
    msg = "{\"action\":\"add\",\"barcode\":#{pBarcode},\"cartId\":#{pCart},\"qty\":#{pQty}}\n"
    puts "--- #{msg} ---"
    @socket.puts(msg)
  end

  def RemoveItem( pBarcode, pCart )
    msg = "{\"action\":\"remove\",\"barcode\":#{pBarcode},\"cartId\":#{pCart}}\n"
    puts "--- #{msg} ---"
    @socket.puts(msg)
  end

  def CloseSocket(  )
    if !@socket.nil?
      @socket.close
      #@thrListen.join
    end
  end


end

