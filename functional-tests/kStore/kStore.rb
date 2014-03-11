require 'rubygems'
require 'httparty'
require '../common/base/JsonSvz.rb'

class KStore < JsonSvz
  attr_reader :totalDiscounts, :totalAmount, :amountToPay
  @reqId = 0
  @devId = 0
  @codiceCarta = " "
  @result = ""
  @totalAmount = 0.0
  @totalDiscounts = 0.0
  @amountToPay = 0.0

  @status = 0
  @deviceReqId = 0


  def InitializeSession( pMyRequestId, pDevId )
    @devId = pDevId

    query = "/initSession?devId=#{@devId}&devReqId=1"
    @result = getResponse( query )
    #puts @result.to_s
    #puts pMyRequestId

    @sessionId[pMyRequestId] = @result["sessionId"]
    @reqId[pMyRequestId] = 1

    #puts @reqId.to_s
    #puts "SessionId: #{@sessionId}"

    @deviceReqId = @result["deviceReqId"]
    @status = @result["status"]
  end

  def RecoverSession( pSessionId, pMyRequestId, pMyReqNum, pDevId )
    @devId = pDevId
    @sessionId[pMyRequestId] = pSessionId
    @reqId[pMyRequestId] = pMyReqNum
    #puts @reqId.to_s
  end

  def AddCustomer( pMyRequestId, pCardCode, pIsVoid )
    @codiceCarta = pCardCode
    @reqId[pMyRequestId] = @reqId[pMyRequestId] + 1
    if pIsVoid
      query = "/voidCustomer?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}&customerId=#{pCardCode}"
    else
      query = "/addCustomer?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}&customerId=#{pCardCode}"
    end
    @result = getResponse( query )
    puts @result.to_s
    @customerID = @result["customerID"]
    @name = @result["name"]
    @loyaltyMessage = @result["loyaltyMessage"]
    @deviceReqId = @result["deviceReqId"]
    @status = @result["status"]
  end

  def AddItem( pMyRequestId, pItemType, pItemCode, pIsVoid )
    @itemType = pItemType
    @itemCode = pItemCode
    #puts @reqId.to_s
    #puts pMyRequestId.to_s
    @reqId[pMyRequestId] = @reqId[pMyRequestId] + 1
    #puts "AddItem Params: #{@itemType} #{@itemCode} #{@reqId[pMyRequestId]}"
    if pIsVoid
      query = "/voidItem?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}&barcode=#{@itemCode}&bcdType=#{@itemType}"
    else
      query = "/addItem?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}&barcode=#{@itemCode}&bcdType=#{@itemType}"
    end
    @result = getResponse( query )
    puts @result.to_s
    if @result["status"] != 3
      @deviceReqId = @result["addItemResponse"]["deviceReqId"]
      @status = @result["addItemResponse"]["status"]
    else
      puts "Errore articolo: #{pItemType}, #{pItemCode}, #{pIsVoid}"
      puts @result.to_s
      @deviceReqId = @result["deviceReqId"]
      @status = @result["status"]
    end
  end

  def GetTotals( pMyRequestId )
    @reqId[pMyRequestId] = @reqId[pMyRequestId] + 1
    query = "/getTotals?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}"
    @result = getResponse( query )
    puts @result.to_s
    @totalAmount = @result["totalAmount"]
    @totalDiscounts = @result["totalDiscounts"]
    @amountToPay = @result["amountToPay"]
    @deviceReqId = @result["deviceReqId"]
    @status = @result["status"]
  end

  def SuspendTransaction( pMyRequestId, pPosNumber )
    @posNumber = pPosNumber
    @reqId[pMyRequestId] = @reqId[pMyRequestId] + 1
    query = "/endSession?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}&payStationID=#{@posNumber}"
    @result = getResponse( query )
    puts @result.to_s
    @deviceReqId = @result["deviceReqId"]
    @status = @result["status"]
  end

  def VoidTransaction( pMyRequestId )
    @reqId[pMyRequestId] = @reqId[pMyRequestId] + 1
    query = "/voidTransaction?devId=#{@devId}&devReqId=#{@reqId[pMyRequestId]}&devSessId=#{@sessionId[pMyRequestId]}"
    @result = getResponse( query )
    puts @result.to_s
    @status = @result["status"]
  end

  def getStatus( )
    return @status
  end

  def getSessionId( pMyRequestId )
    return @sessionId[pMyRequestId]
  end

  def getCustomerId( )
    return @customerID
  end

  def getResult( )
    return @result
  end

end

