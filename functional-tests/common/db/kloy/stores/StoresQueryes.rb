require 'rubygems'
require 'geocoder'
require '../common/db/kloy/KloyDb.rb'
require '../common/base/tools/geo.rb'
require 'yajl'

class StoresQueryes < KloyDb
   @statementGetStoreAddressesWithChanAndId = nil
      
   def initStatements()
      @strGetStoreAddressesWithChanAndId = 'SELECT 
            NEG_DIVISIONE
            ,NEG_CODICE 
            ,NEG_INDIRIZZO||\', \'||NEG_CAP||\' \'||NEG_LOCALITA AS FULL_ADDRESS
         FROM 
            KCRMCNE.TEMP_NEGOZI
         WHERE
            NEG_DIVISIONE NOT IN (18,19)
            AND COORDS_LAT IS NULL
            AND COORDS_LONG IS NULL
         '
      @strUpdateCoord = 'UPDATE KCRMCNE.TEMP_NEGOZI SET COORDS_LAT = ?, COORDS_LONG = ? WHERE NEG_DIVISIONE = ? AND NEG_CODICE = ?'
      @statementGetStoreAddressesWithChanAndId = DB_CONN.prepare( @strGetStoreAddressesWithChanAndId )
      @statementUpdateCoord = DB_CONN.prepare( @strUpdateCoord )
      
      @strGetAllStoresKeys = 'SELECT ID_CMP_FK,ID_DIV_FK,ID_ENT FROM OENTITY ORDER BY ID_CMP_FK,ID_DIV_FK,ID_ENT'
      @statementGetAllStoresKeys = DB_CONN.prepare( @strGetAllStoresKeys )
      
      @strGetStoresWithsCoords = 'SELECT 
	      	OE.ID_CMP_FK
	      	,OE.ADDR_STREET_TYPE_FK
	      	,OE.ADDR_NO
	      	,OE.LNG_DEC
	      	,SC.DESCR AS CITYNAME
	      	,OE.ID_DIV_FK
	      	,OE.ADDR_ZIPCODE
	      	,OE.DESCR
	      	,OE.ID_ENT
	      	,OE.ADDR_STR_NAME
	      	,OE.LAT_DEC
			FROM 
				OENTITY OE
				, SCITY SC
				, SPROVINCE SP
			WHERE
				OE.ADDR_CITY_FK = SC.ID_CITY_FK
				AND OE.ADDR_REGION_FK = SC.ID_REGION_FK
				AND SC.ID_PROVINCE_FK = SP.COD_PROVINCE
				AND SP.ID_PROVINCE = OE.ADDR_PRV_FK
			'
		  @statementstrGetStoresWithsCoords = DB_CONN.prepare( @strGetStoresWithsCoords )
			
			@strGetStoreDetailsByCode = 'SELECT 
        OE.ID_CMP_FK
        ,OE.PHONE1
        ,OE.LNG_DEC
        ,OE.ID_DIV_FK
        ,SC.DESCR AS CITYNAME
        ,OE.DESCR AS SHOPNAME
        ,OE.HOST_ID
        ,SPE.DESCR
        ,OE.LAT_DEC
        ,OE.ADDR_STREET_TYPE_FK
        ,OE.ADDR_NO
        ,OE.ADDR_ZIPCODE
        ,OE.ID_ENT
        ,OE.ADDR_STR_NAME
      FROM 
        OENTITY OE
        , SCITY SC
        , SPROVINCE SP
        , SPARAM_EXT SPE
      WHERE
        OE.ADDR_CITY_FK = SC.ID_CITY_FK
        AND OE.ADDR_REGION_FK = SC.ID_REGION_FK
        AND SC.ID_PROVINCE_FK = SP.COD_PROVINCE
        AND SP.ID_PROVINCE = OE.ADDR_PRV_FK
        AND OE.ID_CMP_FK = ?
        AND OE.ID_DIV_FK = ?
        AND OE.ID_ENT = ?
        and OE.ENT_TYPE_ID_FK=SPE.ID_PARAM_CODE
        and spe.ID_PARAM_KEY=\'EntityType\'
        AND SPE.FLG_STATE=\'0\'
      '
			@statementstGetStoreDetailsByCode = DB_CONN.prepare( @strGetStoreDetailsByCode )

      @strGetStoreServicesByCode = 'SELECT DISTINCT
        SCH.ID_COLL_TYPE
        ,SPE.DESCR_EXT
        ,SCD.ID_PARAM_CODE_FK
        ,SPE.VAL
        ,SPE.DESCR
        ,SPE.ICON_FNAME
      FROM 
        OENTITY OE
        , SPARAM_COLL_HEAD SCH
        , SPARAM_COLL_DTL SCD
        , SPARAM_COLL_XREF SCX
        , SPARAM_EXT SPE
      WHERE
        OE.ID_CMP_FK = SCX.ID_CMP_FK
        AND OE.ID_DIV_FK = SCX.ID_DIV_FK
        AND OE.ID_ENT = SCX.ID_ENT_FK
        AND SCX.ID_COLL_FK = SCH.ID_COLL
        AND SCX.ID_COLL_TYPE = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL = SCD.ID_PARAM_COLL_FK
        AND SPE.ID_PARAM_KEY = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL_TYPE = \'Service\'
        AND SCD.ID_PARAM_CODE_FK=SPE.ID_PARAM_CODE
        AND SCH.FLG_STATE=\'0\'
        AND SCD.FLG_STATE=\'0\' 
        AND SPE.FLG_STATE=\'0\' 
        AND ( ( TRUNC(CURRENT_DATE) >= SCH.DT_START ) OR ( SCH.DT_START IS NULL ) )
        AND ( ( TRUNC(CURRENT_DATE) <= SCH.DT_END ) OR ( SCH.DT_END IS NULL ) )
        AND OE.ID_CMP_FK = ?
        AND OE.ID_DIV_FK = ?
        AND OE.ID_ENT = ?
      ORDER BY 
        lower(SPE.DESCR)
        ,lower(SPE.DESCR_EXT)
      '
      
      @statementstGetStoreServicesByCode = DB_CONN.prepare( @strGetStoreServicesByCode )  
      
      @strGetStoresHavingServiceWithCoords = 'SELECT 
        OE.ID_CMP_FK
        ,OE.ADDR_STREET_TYPE_FK
        ,OE.ADDR_NO
        ,OE.LNG_DEC
        ,SC.DESCR AS CITYNAME
        ,OE.ID_DIV_FK
        ,OE.ADDR_ZIPCODE
        ,OE.DESCR
        ,OE.ID_ENT
        ,OE.ADDR_STR_NAME
        ,OE.LAT_DEC
      FROM 
        OENTITY OE
        , SCITY SC
        , SPROVINCE SP
        , SPARAM_COLL_DTL SCD
        , SPARAM_COLL_XREF SCX
      WHERE
        OE.ADDR_CITY_FK = SC.ID_CITY_FK
        AND OE.ADDR_REGION_FK = SC.ID_REGION_FK
        AND SC.ID_PROVINCE_FK = SP.COD_PROVINCE
        AND SP.ID_PROVINCE = OE.ADDR_PRV_FK
        AND SCD.ID_PARAM_COLL_FK = SCX.ID_COLL_FK
           AND OE.ID_CMP_FK = SCX.ID_CMP_FK
           AND OE.ID_DIV_FK = SCX.ID_DIV_FK
           AND OE.ID_ENT = SCX.ID_ENT_FK
           AND ( ( TRUNC(CURRENT_DATE) >= SCX.DT_START ) OR ( SCX.DT_START IS NULL ) )
           AND ( ( TRUNC(CURRENT_DATE) <= SCX.DT_END ) OR ( SCX.DT_END IS NULL ) )
        AND SCX.ID_COLL_TYPE = \'Service\'
        AND SCD.ID_PARAM_CODE_FK = ?
      '
      @statementGetStoresHavingServiceWithCoords = DB_CONN.prepare( @strGetStoresHavingServiceWithCoords )  
       
      @strGetStoreCalendarByCode = 'SELECT 
        OEC.VALID_TUE
        ,OEC.FLG_CLOSE
        ,OEC.FLG_XTRA_CAL
        ,OEC.TM_RANGE
        ,OEC.FLG_WEEK_END
        ,TO_CHAR(OEC.DT_END,\'YYYY-MM-DD\') AS DT_END
        ,OEC.VALID_FRI
        ,TO_CHAR(OEC.DT_START,\'YYYY-MM-DD\') AS DT_START
        ,OEC.VALID_MON
        ,OEC.VALID_WED
        ,OEC.VALID_SUN
        ,OEC.VALID_THU
        ,OEC.ID_CAL
        ,OEC.VALID_SAT
        ,OEC.FLG_OPEN
      FROM 
        OENTITY OE
        , SPARAM_COLL_HEAD SCH
        , SPARAM_COLL_DTL SCD
        , SPARAM_COLL_XREF SCX
        , OENT_CAL OEC
      WHERE
        OE.ID_CMP_FK = SCX.ID_CMP_FK
        AND OE.ID_DIV_FK = SCX.ID_DIV_FK
        AND OE.ID_ENT = SCX.ID_ENT_FK
        AND SCX.ID_COLL_FK = SCH.ID_COLL
        AND SCX.ID_COLL_TYPE = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL_TYPE = \'Calendar\'
        AND SCH.ID_COLL = SCD.ID_PARAM_COLL_FK
        AND SCD.ID_PARAM_COLL_FK = SCH.ID_COLL
        AND SCD.ID_PARAM_CODE_FK = OEC.ID_CAL
        AND SCH.FLG_STATE=\'0\'
        AND SCD.FLG_STATE=\'0\' 
        AND SCX.FLG_STATE=\'0\'
        AND OEC.FLG_STATE=\'0\'
        --AND ( ( TRUNC(CURRENT_DATE) >= OEC.DT_START ) OR ( OEC.DT_START IS NULL ) )
        --AND ( ( TRUNC(CURRENT_DATE) <= OEC.DT_END ) OR ( OEC.DT_END IS NULL ) )
        AND OE.ID_CMP_FK = ?
        AND OE.ID_DIV_FK = ?
        AND OE.ID_ENT = ?
        --AND OEC.DT_END >= TRUNC(CURRENT_DATE)
      ORDER BY
        OEC.FLG_XTRA_CAL 
        , OEC.DT_START
        , OEC.DT_END 
        , OEC.VALID_MON 
        || OEC.VALID_TUE
        || OEC.VALID_WED
        || OEC.VALID_THU
        || OEC.VALID_FRI
        || OEC.VALID_SAT        
        || OEC.VALID_SUN
        desc
      '
       @statementstGetStoreCalendarByCode = DB_CONN.prepare( @strGetStoreCalendarByCode )      
		 
		 
      @strGetStoreWithSparamExtFindByIdParamKeyStart = 'SELECT DISTINCT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
      FROM 
        OENTITY OE
        , SPARAM_COLL_HEAD SCH
        , SPARAM_COLL_DTL SCD
        , SPARAM_COLL_XREF SCX
        , SPARAM_EXT SPE
      WHERE
        OE.ID_CMP_FK = SCX.ID_CMP_FK
        AND SCH.FLG_STATE=\'0\'
        AND SCD.FLG_STATE=\'0\'  
        AND OE.ID_DIV_FK = SCX.ID_DIV_FK
        AND OE.ID_ENT = SCX.ID_ENT_FK
        AND SCX.ID_COLL_FK = SCH.ID_COLL
        AND SCX.ID_COLL_TYPE = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL = SCD.ID_PARAM_COLL_FK
        AND SPE.ID_PARAM_KEY = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL_TYPE = ?
        AND SCD.ID_PARAM_CODE_FK=SPE.ID_PARAM_CODE '
        
     @strGetStoreWithSparamExtFindByIdParamKeyEnd = ' ORDER BY 
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT'

     
     @strGetStoreWithFutureSparamExtFindByIdParamKey = @strGetStoreWithSparamExtFindByIdParamKeyStart  + ' AND ( TRUNC(CURRENT_DATE) < SCH.DT_START ) AND SPE.FLG_STATE=\'0\' ' + @strGetStoreWithSparamExtFindByIdParamKeyEnd
     @statementstGetStoreWithFutureSparamExtFindByIdParamKey = DB_CONN.prepare( @strGetStoreWithFutureSparamExtFindByIdParamKey ) 
     @strGetStoreWithExpiredSparamExtFindByIdParamKey = @strGetStoreWithSparamExtFindByIdParamKeyStart + ' AND ( TRUNC(CURRENT_DATE) > SCH.DT_END   ) AND SPE.FLG_STATE=\'0\' ' + @strGetStoreWithSparamExtFindByIdParamKeyEnd
     @statementstGetStoreWithExpiredSparamExtFindByIdParamKey = DB_CONN.prepare( @strGetStoreWithExpiredSparamExtFindByIdParamKey ) 

     @strGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey = @strGetStoreWithSparamExtFindByIdParamKeyStart + ' AND ( ( TRUNC(CURRENT_DATE) >= SCH.DT_START ) OR ( SCH.DT_START IS NULL ) ) AND ( ( TRUNC(CURRENT_DATE) <= SCH.DT_END ) OR ( SCH.DT_END IS NULL ) ) AND SPE.FLG_STATE<>\'0\' '  + @strGetStoreWithSparamExtFindByIdParamKeyEnd
      
     @statementstGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey = DB_CONN.prepare( @strGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey )

      @strGetKcmCoopStoreIdMapping = 'SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      ORDER BY
        OE.HOST_ID'
      @statementGetKcmCoopStoreIdMapping = DB_CONN.prepare( @strGetKcmCoopStoreIdMapping )

      @strGetKcmCoopStoreIdMappingByIdCmpFk = 'SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_CMP_FK = ?
      ORDER BY
        OE.HOST_ID'
      @statementGetKcmCoopStoreIdMappingByIdCmpFk = DB_CONN.prepare( @strGetKcmCoopStoreIdMappingByIdCmpFk )

      @strGetKcmCoopStoreIdMappingByIdDivFk = 'SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_CMP_FK = ?
      ORDER BY
        OE.HOST_ID'
      @statementGetKcmCoopStoreIdMappingByIdDivFk = DB_CONN.prepare( @strGetKcmCoopStoreIdMappingByIdDivFk )

      @strGetKcmCoopStoreIdMappingByIdCmpFkIdDivFk = 'SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_CMP_FK = ?
        AND OE.ID_CMP_FK = ?
      ORDER BY
        OE.HOST_ID'
      @statementGetKcmCoopStoreIdMappingByIdCmpFkIdDivFk = DB_CONN.prepare( @strGetKcmCoopStoreIdMappingByIdCmpFkIdDivFk )

     #puts @strGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey ;
   end

   def functGetStoresWithsCoords( )
     functGetStoresByCoords( @statementstrGetStoresWithsCoords, nil )
   end

   def functGetStoresHavingServiceWithsCoords( pService )
     functGetStoresByCoords( @statementstrGetStoresWithsCoords, pService )
   end
      
   def functGetStoresByCoords( pStatementForStores, pService )
      values = 0
      count = 0
      
      storesList = Hash.new
      if pService.nil?
        pStatementForStores.execute()
      else  
        pStatementForStores.execute( pService )
      end
      
      pStatementForStores.fetch do |row|
         count = count + 1
         values = { "ID_CMP_FK" => getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "ADDR_STREET_TYPE_FK" => getBlankOrValue( row["ADDR_STREET_TYPE_FK"] ) ,
                    "ADDR_NO" => getBlankOrValue( row["ADDR_NO"] ) ,
                    "LNG_DEC" => getBlankOrValue( row["LNG_DEC"] ) ,
                    "CITYNAME" => getBlankOrValue( row["CITYNAME"] ) ,
                    "ID_DIV_FK" => getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "ADDR_ZIPCODE" => getBlankOrValue( row["ADDR_ZIPCODE"] ) ,
                    "DESCR" => getBlankOrValue( row["DESCR"] ) ,
                    "ID_ENT" => getBlankOrValue( row["ID_ENT"] ) ,
                    "ADDR_STR_NAME" => getBlankOrValue( row["ADDR_STR_NAME"] ) ,
                    "LAT_DEC" => getBlankOrValue( row["LAT_DEC"] ) ,
            }   	
        #puts values.to_s
        idEntCode = row["ID_DIV_FK"] << "_" << row["ID_ENT"]
        storesList[ idEntCode ] = values    
        end
        return storesList
   end
     
   def functGetStoresWithsCoordsInRange( pLat, pLng, pDist )
      storesInRange = Hash.new
      sortedStoresByDb = Hash.new
      stores = functGetStoresWithsCoords( )
      stores.each do | store |
        dist = getDistanceBetweenCoords( pLat.to_f,pLng.to_f, store[1]["LAT_DEC"].to_f, store[1]["LNG_DEC"].to_f )
        if dist < ( pDist.to_f * 1000 )
          storesInRange[store[0]] = store[1] 
          storesInRange[store[0]]["DIST_FROM_POS"] = dist.to_i
        end 
      end
      sortedStoresByDb = storesInRange.sort_by { |hsh| [hsh[1]["DIST_FROM_POS"],hsh[1]["DESCR"]] }
   end

   def functGetStoresWithsCoordsInRangeMaxResult( pLat, pLng, pDist, pNumResults )
      storesByDb = functGetStoresWithsCoordsInRange( pLat, pLng, pDist )
      storeCount = 1  
      storesInRange = Hash.new
      storesByDb.each do | store |
        storesInRange[store[0]] = store[1]
        storeCount += 1
        if storeCount > pNumResults.to_i then 
           break
        end
      end
      return storesInRange
   end
   
      
   def funcGetStoreAddressesWithChanAndId ( )
      values = 0
      count = 0
      @statementGetStoreAddressesWithChanAndId.execute()
      @statementGetStoreAddressesWithChanAndId.fetch do |row|
         count = count + 1
         values = { "NEG_DIVISIONE" => getBlankOrValue( row["NEG_DIVISIONE"] ), 
                    "NEG_CODICE" => getBlankOrValue( row["NEG_CODICE"] ), 
                    "FULL_ADDRESS" => getBlankOrValue( row["FULL_ADDRESS"] ), 
            }
         s = Geocoder.search(values["FULL_ADDRESS"] )
         puts "\r\n================================================" << count.to_s
         puts values["FULL_ADDRESS"]
         sleep 2
         unless s[0].nil?
            addr = s[0].data
            puts "Lat: " << addr["geometry"]["location"]["lat"].to_s
            puts "Lng: " << addr["geometry"]["location"]["lng"].to_s
            @statementUpdateCoord.execute( addr["geometry"]["location"]["lat"].to_s, addr["geometry"]["location"]["lng"].to_s, values["NEG_DIVISIONE"], values["NEG_CODICE"] )
            DB_CONN.commit
         end
      end
      return values
   end
   
   def funcGetStoreServicesByCode( pIdCmpFk, pIdDivFk, pIdEntFk )
      count = 0
      
      storeServices = Array.new
      
      @statementstGetStoreServicesByCode.execute( pIdCmpFk, pIdDivFk, pIdEntFk )
      
      @statementstGetStoreServicesByCode.fetch do |row|
         count = count + 1
         storeService = { "IdParamKey" => getBlankOrValue( row["ID_COLL_TYPE"] ) ,
                    "DescrExt" => getBlankOrValue( row["DESCR_EXT"] ) ,
                    "IdParamCode" => getBlankOrValue( row["ID_PARAM_CODE_FK"] ) ,
                    "Val" => getBlankOrValue( row["VAL"] ) ,
                    "Descr" => getBlankOrValue( row["DESCR"] ) ,
                    "IconFname" => getBlankOrValue( row["ICON_FNAME"] ) ,
            }
         storeServices.push( storeService )    
      end
      
      return storeServices
   end
   
   
   def funcGetStoreDetailServicesCalendarByCode( pIdCmpFk, pIdDivFk, pIdEntFk )
      values = 0
      count = 0
      
      storeDetailServicesCalendar = Hash.new
      storeDetails = nil
      storeServices = Array.new
      storeCalendars = Array.new
      storeCalendarsExtra = Array.new
      @statementstGetStoreDetailsByCode.execute( pIdCmpFk, pIdDivFk, pIdEntFk )

      @statementstGetStoreDetailsByCode.fetch do |row|
         count = count + 1
         storeDetails = { "IdCmpFk" => getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "Phone1" => getBlankOrValue( row["PHONE1"] ) ,
                    "LngDec" => getBlankOrValue( row["LNG_DEC"] ) ,
                    "IdDivFk" => getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "CityName" => getBlankOrValue( row["CITYNAME"] ) ,
                    "ShopName" => getBlankOrValue( row["SHOPNAME"] ) ,
                    "HostId" => getBlankOrValue( row["HOST_ID"] ) ,
                    "EntTypeDescr" => getBlankOrValue( row["DESCR"] ) ,
                    "LatDec" => getBlankOrValue( row["LAT_DEC"] ) ,
                    "AddrStreetType" => getBlankOrValue( row["ADDR_STREET_TYPE_FK"] ) ,
                    "AddrNo" => getBlankOrValue( row["ADDR_NO"] ) ,
                    "AddrZipcode" => getBlankOrValue( row["ADDR_ZIPCODE"] ) ,
                    "IdEnt" => getBlankOrValue( row["ID_ENT"] ) ,
                    "AddrStrName" => getBlankOrValue( row["ADDR_STR_NAME"] ) ,
            }  
        #puts "ererer" << Yajl::Encoder.encode( getBlankOrValue( row["ADDR_STR_NAME"] ) )
       end
        
      storeServices = funcGetStoreServicesByCode( pIdCmpFk, pIdDivFk, pIdEntFk )
       
      rowExtraCal = 0

      @statementstGetStoreCalendarByCode.execute( pIdCmpFk, pIdDivFk, pIdEntFk )      
      
      @statementstGetStoreCalendarByCode.fetch do |row|
         count = count + 1
         dtStart = Date.parse( row["DT_START"] )
         dtEnd = Date.parse( row["DT_END"] )
         extraCal = getBlankOrValue( row["FLG_XTRA_CAL"] ) == '1'

         addCal = false
         if Date.today <= dtEnd
           if !extraCal 
             if dtStart <= Date.today
                addCal = true
             end   
           else 
             addCal = true
             rowExtraCal = rowExtraCal + 1
           end  
         end
         #puts "--- " << addCal.to_s << " " << extraCal.to_s << " " << rowExtraCal.to_s ;
         if ( addCal && !extraCal) || ( addCal && extraCal && rowExtraCal < 2 )

           storeCalendar = {  "ValidTue" => getBlankOrValue( row["VALID_TUE"] ) ,
                      "FlgClose" => getBlankOrValue( row["FLG_CLOSE"] ) ,
                      "FlgXtraCal" => getBlankOrValue( row["FLG_XTRA_CAL"] ) ,
                      "TmRange" => getBlankOrValue( row["TM_RANGE"] ) ,
                      "FlgWeekEnd" => getBlankOrValue( row["FLG_WEEK_END"] ) ,
                      "DtEnd" => getBlankOrValue( row["DT_END"] ).to_s.slice(0..9) ,
                      "ValidFri" => getBlankOrValue( row["VALID_FRI"] ) ,
                      "DtStart" => getBlankOrValue( row["DT_START"] ).to_s.slice(0..9) ,
                      "ValidMon" => getBlankOrValue( row["VALID_MON"] ) ,
                      "ValidWed" => getBlankOrValue( row["VALID_WED"] ) ,
                      "ValidSun" => getBlankOrValue( row["VALID_SUN"] ) ,
                      "ValidThu" => getBlankOrValue( row["VALID_THU"] ) ,
                      "IdCal" => getBlankOrValue( row["ID_CAL"] ) ,
                      "ValidSat" => getBlankOrValue( row["VALID_SAT"] ) ,
                      "FlgOpen" => getBlankOrValue( row["FLG_OPEN"] ) ,
              }
              
           if !extraCal 
             storeCalendars.push( storeCalendar ) 
           else
             storeCalendarsExtra.push( storeCalendar ) 
             #puts "addExtra"
           end
         end         
       end
      storeDetailServicesCalendar["ShopInfo"] = storeDetails
      storeDetailServicesCalendar["ServicesList"] = storeServices
      storeDetailServicesCalendar["CalendarList"] = storeCalendars
      storeDetailServicesCalendar["CalendarExtraList"] = storeCalendarsExtra
      #puts "CalendarList: " << storeDetailServicesCalendar["CalendarList"].to_s
      #puts "CalendarExtraList: " << storeDetailServicesCalendar["CalendarExtraList"].to_s
      return storeDetailServicesCalendar    
   end
   
   
   
   def funcGetAllStoresByKey( )
      values = 0
      @statementGetAllStoresKeys.execute( )
      storeList = Array.new
     
      @statementGetAllStoresKeys.fetch do |row|
         store = { "ID_CMP_FK" =>  getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "ID_DIV_FK" =>  getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "ID_ENT" =>  getBlankOrValue( row["ID_ENT"] ) ,
            }
         storeList.push( store )      
      end
      #puts storeList.to_s
      return storeList
   end
   
   def funcGetAllStoresWithFutureSparamExtFindByIdParamKey( pIdParamKey )
      values = 0
      @statementstGetStoreWithFutureSparamExtFindByIdParamKey.execute( pIdParamKey )
      storeList = Array.new
     
      @statementstGetStoreWithFutureSparamExtFindByIdParamKey.fetch do |row|
         store = { "ID_CMP_FK" =>  getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "ID_DIV_FK" =>  getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "ID_ENT" =>  getBlankOrValue( row["ID_ENT"] ) ,
            }
         storeList.push( store )      
      end
      #puts storeList.to_s
      return storeList
   end
   
   def funcGetAllStoresWithExpiredSparamExtFindByIdParamKey( pIdParamKey )
      values = 0
      @statementstGetStoreWithExpiredSparamExtFindByIdParamKey.execute( pIdParamKey )
      storeList = Array.new
     
      @statementstGetStoreWithExpiredSparamExtFindByIdParamKey.fetch do |row|
         store = { "ID_CMP_FK" =>  getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "ID_DIV_FK" =>  getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "ID_ENT" =>  getBlankOrValue( row["ID_ENT"] ) ,
            }
         storeList.push( store )      
      end
      #puts storeList.to_s
      return storeList
   end

   def funcGetAllStoresWithVoidedSparamExtValidByDateFindByIdParamKey( pIdParamKey )
      values = 0
      @statementstGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey.execute( pIdParamKey )
      storeList = Array.new
     
      @statementstGetStoreWithCurrentButVoidedSparamExtFindByIdParamKey.fetch do |row|
         store = { "ID_CMP_FK" =>  getBlankOrValue( row["ID_CMP_FK"] ) ,
                    "ID_DIV_FK" =>  getBlankOrValue( row["ID_DIV_FK"] ) ,
                    "ID_ENT" =>  getBlankOrValue( row["ID_ENT"] ) ,
            }
         storeList.push( store )      
      end
      #puts storeList.to_s
      return storeList
   end
   
   def funcGetAllServicesByStoreIds ( pStoreids )
    @strGetAllServicesByStoreIds = "  SELECT DISTINCT
        SCD.ID_PARAM_CODE_FK
      FROM 
        OENTITY OE
        , SPARAM_COLL_HEAD SCH
        , SPARAM_COLL_DTL SCD
        , SPARAM_COLL_XREF SCX
        , SPARAM_EXT SPE
      WHERE
        OE.ID_CMP_FK = SCX.ID_CMP_FK
        AND OE.ID_DIV_FK = SCX.ID_DIV_FK
        AND OE.ID_ENT = SCX.ID_ENT_FK
        AND SCX.ID_COLL_FK = SCH.ID_COLL
        AND SCX.ID_COLL_TYPE = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL = SCD.ID_PARAM_COLL_FK
        AND SPE.ID_PARAM_KEY = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL_TYPE = \'Service\'
        AND SCD.ID_PARAM_CODE_FK=SPE.ID_PARAM_CODE
        AND SCH.FLG_STATE=\'0\'
        AND SCD.FLG_STATE=\'0\' 
        AND SPE.FLG_STATE=\'0\' 
        AND ( ( TRUNC(CURRENT_DATE) >= SCH.DT_START ) OR ( SCH.DT_START IS NULL ) )
        AND ( ( TRUNC(CURRENT_DATE) <= SCH.DT_END ) OR ( SCH.DT_END IS NULL ) )
        AND OE.ID_CMP_FK||OE.ID_DIV_FK||OE.ID_ENT IN (#{pStoreids})"
           
      rows = DB_CONN.execute( @strGetAllServicesByStoreIds )
      
      services = Array.new
      rows.fetch do |row|
         idParamCodeFk = getBlankOrValue( row["ID_PARAM_CODE_FK"] )
         services.push idParamCodeFk
      end  
      return services
   end

   def funcGetKcmCoopStoreIdMapping( pCompanyId, pDivisionId )
     values = 0

     if pCompanyId.nil? && pDivisionId.nil?
       strQuery = "SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      ORDER BY
        OE.HOST_ID"
     end

     if !pCompanyId.nil? && pDivisionId.nil?
       strQuery = "SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_CMP_FK = #{pCompanyId}
      ORDER BY
        OE.HOST_ID"
     end

     if !pDivisionId.nil? && pCompanyId.nil?
       strQuery = "SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_DIV_FK = #{pDivisionId}
      ORDER BY
        OE.HOST_ID"
     end

     if !pDivisionId.nil? && !pCompanyId.nil?
       strQuery = "SELECT
        OE.ID_CMP_FK
        , OE.ID_DIV_FK
        , OE.ID_ENT
        , OE.HOST_ID
        , OE.DESCR
      FROM
        OENTITY OE
      WHERE
        OE.ID_CMP_FK = #{pCompanyId}
        AND OE.ID_DIV_FK = #{pDivisionId}
      ORDER BY
        OE.HOST_ID"
     end

     #puts strQuery
     statement = DB_CONN.prepare( strQuery )
     statement.execute

     storeList = Array.new

     statement.fetch do |row|
       store = { "IdCmpFk" =>  getBlankOrValue( row["ID_CMP_FK"] ) ,
                 "IdDivFk" =>  getBlankOrValue( row["ID_DIV_FK"] ) ,
                 "IdEnt" =>  getBlankOrValue( row["ID_ENT"] ) ,
                 "HostId" =>  getBlankOrValue( row["HOST_ID"] ) ,
                 "Descr" =>  getBlankOrValue( row["DESCR"] ) ,
       }
       storeList.push( store )
     end
     #puts storeList.to_s
     return storeList
   end

   def closeStatements()
      @strGetStoreAddressesWithChanAndId.finish
      @statementUpdateCoord.finish
      @statementstrGetStoresWithsCoords.finish
   end
   
   
   def close()
	   closeStatements()
   end
      
end


