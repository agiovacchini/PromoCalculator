require 'net/http'
require 'json/pure'

class JsonSvz

  @urlBase = ''
  @query = ''
  @respType = ''
  @rc = -1
  @rm = '-1'

  def initialize( pUrlBase, pRespType )
    @urlBase = pUrlBase
    @respType = pRespType
    @rc = -1
    @rm = '-1'
    @sessionId = Hash.new
    @reqId = Hash.new
  end

  def getResponse( pQuery )

    @query = pQuery
    url = "http://#{@urlBase}#{@query}"
    #puts "Url per la richiesta: " << url
    resp = Net::HTTP.get_response( URI.parse( URI.encode( url.strip ) ) )
    data = resp.body
    #puts data
    #puts URI.parse( URI.encode( url.strip ) )
    case @respType
      when "JSON"
        result = JSON.parse( data )
      when
      result  = Nokogiri::XML( data )
      else
        result = nil
    end


  end

  def getResponseWithAuth( pQuery, pUser, pPass )
    @query = pQuery

    url = URI.parse( "http://#{@urlBase}" )

    req = Net::HTTP::Post.new(url.path)
    req.basic_auth pUser, pPass

    #puts "Url per la richiesta: " << url.path
    resp = Net::HTTP.new(url.host, url.port).start {|http| http.request(req) }
    #puts "@@@@@@@" << resp.to_s << "@@@@@@@"
    #resp = Net::HTTP.get_response( URI.parse( URI.encode( url.strip ) ) )
    data = resp.body
    result = JSON.parse( data )
  end

  def validateResult( )
    if result.has_key? 'Error'
      return false
    end
  end

  def getRc( )
    return @rc
  end

  def getRm( )
    return @rm
  end


end
