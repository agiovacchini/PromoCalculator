require '../common/db/kloy/KloyDb.rb'

class ParamsQueryes < KloyDb
   
   @strSparamExtFindByIdParamKey = nil
      
   def initStatements()
   
      @strSparamExtFindByIdParamKey = 'SELECT DISTINCT
        SPE.ID_PARAM_CODE
        ,SPE.VAL
        ,SPE.DESCR
        ,SPE.DESCR_EXT
        ,SPE.ICON_FNAME
      FROM
        SPARAM_EXT SPE
           , SPARAM_COLL_HEAD SCH
           , SPARAM_COLL_DTL SCD
           , SPARAM_COLL_XREF SCX
           , OENTITY OE
      WHERE
        SPE.ID_PARAM_KEY  = ?
        AND SCH.FLG_STATE=\'0\'
        AND SCD.FLG_STATE=\'0\' 
        AND SPE.FLG_STATE=\'0\' 
        AND SPE.ID_PARAM_KEY = SCH.ID_COLL_TYPE
        AND SCH.ID_COLL = SCD.ID_PARAM_COLL_FK
        AND SCD.ID_PARAM_CODE_FK = SPE.ID_PARAM_CODE
        AND ( ( CURRENT_DATE >= SCH.DT_START ) OR ( SCH.DT_START IS NULL ) )
        AND ( ( CURRENT_DATE <= SCH.DT_END ) OR ( SCH.DT_END IS NULL ) )
        AND SCX.ID_COLL_FK = SCH.ID_COLL
        AND SCX.ID_COLL_TYPE = SCH.ID_COLL_TYPE
        AND OE.ID_CMP_FK = SCX.ID_CMP_FK
        AND OE.ID_DIV_FK = SCX.ID_DIV_FK
        AND OE.ID_ENT = SCX.ID_ENT_FK
      ORDER BY 
        lower( DESCR )'
      
      if DB_TYPE == 'ORA'
      end

      if DB_TYPE == 'DB2'
      end
          
      @statementSparamExtFindByIdParamKey = DB_CONN.prepare(@strSparamExtFindByIdParamKey)
      puts self.class.to_s << " - Statements initialized"
   end

   def funcSparamExtFindByIdParamKey( pIdParamKey )
      values = 0
      @statementSparamExtFindByIdParamKey.execute( pIdParamKey )
      serviceList = Hash.new
     
      @statementSparamExtFindByIdParamKey.fetch do |row|
         service = { "VAL" =>  getBlankOrValue( row["VAL"] ) ,
                    "DESCR" =>  getBlankOrValue( row["DESCR"] ) ,
                    "DESCR_EXT" =>  getBlankOrValue( row["DESCR_EXT"] ) ,
                    "ICON_FNAME" => getBlankOrValue( row["ICON_FNAME"] ) ,
            }
         idParamCode = row["ID_PARAM_CODE"]
         serviceList [ idParamCode ] = service       
      end
      #puts serviceList.to_s
      return serviceList
   end

   def getParamKeys pQuery, pPreText, pPostText
     grouper = ""
     prevGrouper = ""
     rowCount = 0
     resultSet = Hash.new {|h,k| h[k]=[]}
     resultSetRow = Hash.new
    #rows = DB_CONN.execute("select id_chan_fk as GROUPER,ID_PROMO_TYPE as KEY,descr as DESCR from spromo_type order by id_promo_type")
     rows = DB_CONN.execute(pQuery)

     rows.each do |row|
      id = row["KEY"]
      grouper = row["GROUPER"]
      descr = row["DESCR"]
      if !prevGrouper.eql? grouper
        rowCount = 0
      else
        rowCount = rowCount + 1
      end
      value = "#{pPreText}#{rowCount}#{pPostText}"
      #key = "#{grouper} --- #{value} --- #{id} --- #{descr}"
      #puts key
      resultSetRow = { "key" => id, "value" => value}
      if resultSet[descr].nil?
        resultSet[descr] = resultSetRow
      else
        resultSet[descr] << resultSetRow
      end
      prevGrouper = grouper
     end

    return resultSet

   end

   def closeStatements()
      @statementSparamExtFindByIdParamKey.finish
      puts self.class.to_s << " - Statements closed"
   end
   
   
   def close()
   	closeStatements()
   end
      
end
