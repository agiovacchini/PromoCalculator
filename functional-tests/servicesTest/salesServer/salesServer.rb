require 'rubygems'
require 'httparty'
require 'socket'
require 'json'

class SalesServer

  @socket = nil
  @thrListen = nil
  @msgReceived=0
  @msgText=""

  Thread.abort_on_exception = true

  def InitializeSession( pIp, pPort )
    puts "ip: #{pIp}, port: #{pPort}"
    @socket = TCPSocket.open(pIp, pPort)
    @thrListen = Thread.new {
      while 0!=1
        puts "Aspetto messaggio"
        @msgText = @socket.gets
        @msgReceived = 1
      end
    }
  end

  def sendMsg( pMsg )
    @msgReceived=0
    puts "Sending message: #{pMsg}"
    @socket.puts(pMsg)
    while ( @msgReceived==0 )
      sleep 0.1
    end
  end

  def OpenCart( )
    msg = "{\"action\":\"init\"}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def PrintCart( pCart )
    msg = "{\"action\":\"print\",\"cartId\":#{pCart}}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def PersistCart( pCart )
    msg = "{\"action\":\"save\",\"cartId\":#{pCart}}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def SendToPos( pCart, pPosNumber )
    msg = "{\"action\":\"sendToPos\",\"cartId\":#{pCart},\"posNumber\":#{pPosNumber}}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def AddItem( pQty, pBarcode, pCart )
    msg = "{\"action\":\"add\",\"barcode\":#{pBarcode},\"cartId\":#{pCart},\"qty\":#{pQty}}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def RemoveItem( pBarcode, pCart )
    msg = "{\"action\":\"remove\",\"barcode\":#{pBarcode},\"cartId\":#{pCart}}\n"
    sendMsg( msg )
    return JSON.parse(@msgText)
  end

  def CloseSocket(  )
    if !@socket.nil?
      @socket.close
      #@thrListen.join
    end
  end


end

