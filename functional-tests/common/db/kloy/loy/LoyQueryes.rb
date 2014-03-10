require '../common/db/kloy/KloyDb.rb'

class LoyQueryes < KloyDb
   
   @statementLoyFindValidCardAndHashFromOcustMktg = nil
      
   def initStatements()

     @strOacmCountById = 'SELECT * FROM OACM WHERE ID_ACM = ?'
     @statementOacmCountById = DB_CONN.prepare(@strOacmCountById)
     @strOacmInsertRow = 'INSERT INTO OACM (ID_ACM,DESCR,GP_ACM_TYPE_FK,CHAN_FK,DT_START,DT_END,EXT_CODE_REF,FLG_BURN,LAYOUT_CODE,MAX_NUM,FLG_DPL,FLG_DEF,FLG_STATE,USR_INS,TS_INS,USR_DEL,TS_DEL,USR_UPD,TS_UPD,VER_NO,ORD_NO,CPCCCHK,TS_DPL) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)'
     @statementOacmInsertRow = DB_CONN.prepare(@strOacmInsertRow)

     @strLoyFindValidCardAndHashFromOcustMktg = 'select
         ocm.ID_CUST_FK
         ,ocm.EMAIL
         ,ocm.CHAN_NEWSLTR_FK
         ,ocm.ENT_NEWSLTR_FK
         ,oca.ID_CARD
         ,ocm.PWDHASH
         ,ocm.DT_END_PWD
      from
         OCUST_MKTG ocm
         ,OCARD oca
      where
         ocm.ID_CUST_FK=oca.ID_CUST_FK
         AND oca.DT_BLKLST IS NULL
         AND OCA.FLG_STATE = ''0'' '
         
      @strLoyFindValidCardAndHashFromOcustMktg = @strLoyFindValidCardAndHashFromOcustMktg + 'and ocm.DT_END_PWD >= current_date '
      @strLoyFindExpiredCardAndHashFromOcustMktg = @strLoyFindValidCardAndHashFromOcustMktg + 'and ocm.DT_END_PWD < current_date '
      
      @strLoyFindCardNotExistingInOcustMktg = 'select \'0260\'||id||\'01\' as ID_CARD,\'DUMMY_HASH\' as PWDHASH from test_all_card_nums where \'0260\'||id||\'01\' not in ( select ID_CARD from OCUST_MKTG mk, ocard oca where mk.ID_CUST_FK = oca.ID_CUST_FK )'

      #puts @strLoyFindCardNotExistingInOcustMktg
      
      if DB_TYPE== 'ORA'
         @strLoyFindValidCardAndHashFromOcustMktg = @strLoyFindValidCardAndHashFromOcustMktg + ' and rownum = 1 '
         @strLoyFindExpiredCardAndHashFromOcustMktg = @strLoyFindExpiredCardAndHashFromOcustMktg + ' and rownum = 1 '
         @strLoyFindCardNotExistingInOcustMktg = @strLoyFindCardNotExistingInOcustMktg + ' and rownum = 1 '
      end

      if DB_TYPE == 'DB2'
         @strLoyFindValidCardAndHashFromOcustMktg = @strLoyFindValidCardAndHashFromOcustMktg + ' fetch first row only '
         @strLoyFindExpiredCardAndHashFromOcustMktg = @strLoyFindExpiredCardAndHashFromOcustMktg + ' fetch first row only '
         @strLoyFindCardNotExistingInOcustMktg = @strLoyFindCardNotExistingInOcustMktg + ' fetch first row only '
      end
          
      #puts @strLoyFindValidCardAndHashFromOcustMktg
      @statementLoyFindValidCardAndHashFromOcustMktg = DB_CONN.prepare(@strLoyFindValidCardAndHashFromOcustMktg)
      @statementLoyFindExpiredCardAndHashFromOcustMktg = DB_CONN.prepare(@strLoyFindExpiredCardAndHashFromOcustMktg)
      @statementLoyFindCardNotExistingInOcustMktg = DB_CONN.prepare(@strLoyFindCardNotExistingInOcustMktg)

      @strLoyBalanceForCard = 'SELECT
        ID_ACM
        ,COALESCE(ACM_VAL,0) AS ACM_VAL
        ,CONVERSION_CODE
      FROM (
        SELECT
          OCA1.ID_CARD
          ,OAC1.ID_ACM
          ,OCA1.ID_CUST_FK
          ,P.CONVERSION_CODE AS CONVERSION_CODE
        FROM
          OCARD OCA1
          ,OACM OAC1 LEFT OUTER JOIN SGENERIC_PAR P ON OAC1.GP_ACM_TYPE_FK=P.ID_GENERIC_PAR AND P.ID_REC_TYPE=\'AccumulatorType\'
		      ,SLOY_CHAN_XREF SCX1
        WHERE
          ( CURRENT_DATE >= OAC1.DT_START ) AND ( CURRENT_DATE <= OAC1.DT_END )
          AND OCA1.CHAN_FK = SCX1.ID_CHAN
          AND SCX1.ID_LOY = OAC1.CHAN_FK
          AND ID_CARD = ?
      ) SEL1 LEFT OUTER JOIN OBALANCE OBA ON OBA.ACM_FK = SEL1.ID_ACM AND OBA.CARD_FK = SEL1.ID_CUST_FK
      '

      @statementLoyBalanceForCard = DB_CONN.prepare(@strLoyBalanceForCard)

      @strVoucherListForCard = 'select
        OVO.ID_CARD as IdCard
        ,OVO.ID_VOUCHER as IdVoucher
        ,trim(omh.DESCR) as HeaderMsg
        ,OVO.ID_TYPE as IdType
        ,TO_CHAR(OVO.exp_date,\'YYYY-MM-DD\') AS ExpDate
        ,(CASE
          WHEN OVO.ID_TYPE = \'V\' THEN OVO.VAL_EURO
          WHEN OVO.ID_TYPE = \'C\' THEN (select to_number(descr) from omsg_dtl omdtt where omdtt.ID_MSG_FK=ovo.ID_MSG_FK and omdtt.id_seq=1)
         END) as Value
        ,p.descr as ValueType
        ,pv.descr as VoucherType
        ,OCU.HOST_ID as HostId
        ,OCU.ID_CUSTOMER as IdCustomer
        ,oca.FLG_PSS as FlgPss
        ,oca.FLG_BLKLST as FlgBlklst
        ,OCA.FLG_STATE as FlgState
        ,ovo.ID_MSG_FK
      from
        OVOUCHER OVO left outer join kcrmcne.SGENERIC_PAR pv on ovo.id_type=pv.id_generic_par and pv.id_rec_type=\'VoucherType\'
        ,OCARD OCA
        ,ocustomer ocu
        ,omsg_dtl omd left outer join kcrmcne.SGENERIC_PAR p on omd.id_msg_type=p.id_generic_par and p.id_rec_type=\'MsgType\'
        ,OMSG_HDR omh
      WHERE
        OCA.ID_CARD = SUBSTR(OVO.ID_CARD,1,12)
        AND OCA.ID_CUST_FK=OCU.ID_CUSTOMER
        AND OVO.ID_STATE_FK =\'E\'
        AND OVO.ID_MSG_FK IS NOT NULL
        and oca.ID_CARD = ?
        and ovo.exp_date >= current_date
        and ovo.RED_END_DATE >= current_date
        and omd.ID_MSG_FK=ovo.ID_MSG_FK
        and omd.ID_MSG_TYPE in (\'002\',\'003\')
        and ovo.ID_MSG_FK=omh.ID_MSG
        ORDER BY OVO.ID_cARD,OVO.ID_TYPE,OVO.RED_END_DATE'

     @statementVoucherListForCard = DB_CONN.prepare(@strVoucherListForCard)


     @strPssEnabledCards = 'select
       oca.ID_CARD as ID_CARD
     from
       ocard oca
       ,ocustomer ocu
     where
       oca.ID_CUST_FK=ocu.ID_CUSTOMER
       and oca.FLG_BLKLST != \'1\'
       and ID_CARD in (
        select ID_CARD_FK from OPSS_CUST_CARD
       ) and ID_CARD not in (
        select ID_CARD_FK from OBLK_CARD
       )
       and rownum <= ?
      ORDER BY DBMS_RANDOM.RANDOM'

     @statementPssEnabledCards = DB_CONN.prepare(@strPssEnabledCards)

   end

   def funcLoyGetPssEnabledCards( pNumRows )
     values = Array.new
     @statementPssEnabledCards.execute( pNumRows )

     @statementPssEnabledCards.fetch do |row|
       values.push( getBlankOrValue( row["ID_CARD"] ) )
     end

     return values
   end

   def funcLoyFindCardAndHashFromOcustMktg( pStmt )
      #puts 'funcLoyFindCardAndHashFromOcustMktg - start'
      values = 0
      pStmt.execute()
      pStmt.fetch do |row|
         values = { "ID_CARD" => getBlankOrValue( row["ID_CARD"] ) , 
                    "PWDHASH" => getBlankOrValue( row["PWDHASH"] ) , 
                    "EMAIL" => getBlankOrValue( row["EMAIL"] ) , 
            }
         
      end
      #puts 'funcLoyFindCardAndHashFromOcustMktg - end'


      return values
   end

   def funcLoyFindValidCardAndHashFromOcustMktg()
      return funcLoyFindCardAndHashFromOcustMktg @statementLoyFindValidCardAndHashFromOcustMktg
   end
      
   def funcLoyFindExpiredCardAndHashFromOcustMktg()
      return funcLoyFindCardAndHashFromOcustMktg @statementLoyFindExpiredCardAndHashFromOcustMktg
   end
   
   def funcLoyFindCardNotExistingInOcustMktg
      return funcLoyFindCardAndHashFromOcustMktg @statementLoyFindCardNotExistingInOcustMktg
   end

   def getBalanceForCard( pIdCard )
     values = Hash.new
     @statementLoyBalanceForCard.execute( pIdCard )
     @statementLoyBalanceForCard.fetch do |row|
       #puts row.to_s
       values[row["ID_ACM"]] = { "VALUE" => getBlankOrValue( row["ACM_VAL"].to_i ) ,
                                 "TYPEVS" => getBlankOrValue( row["CONVERSION_CODE"].to_s ) ,
                                 "CHECK" => 1 ,
       }
     end
     #puts values.to_s
     return values
   end

   def getVoucherListForCard( pIdCard )
     values = Hash.new
     if pIdCard.length == 13
       pIdCard = pIdCard[0..11]
     end
     @statementVoucherListForCard.execute( pIdCard )
     @statementVoucherListForCard.fetch do |row|
       #puts row.to_s
       values[row["IDVOUCHER"].to_s] = { "IDVOUCHER" => getBlankOrValue( row["IDVOUCHER"].to_s ) ,
                                     "HEADERMSG" => getBlankOrValue( row["HEADERMSG"].to_s ) ,
                                     "IDTYPE" => getBlankOrValue( row["IDTYPE"].to_s ) ,
                                     "VOUCHERTYPE" => getBlankOrValue( row["VOUCHERTYPE"].to_s ) ,
                                     "VALUE" => getBlankOrValue( row["VALUE"].to_f ) ,
                                     "VALUETYPE" => getBlankOrValue( row["VALUETYPE"].to_s ) ,
                                     "EXPDATE" => getBlankOrValue( row["EXPDATE"].to_s ) ,
       }
     end
     return values
   end

   def cleanFiscalCodeSelfScanData
     DB_CONN.do("delete from ocard where ID_CUST_FK in ( select ID_CUST_FK from ocard_alias where ID_CUST_FK like \'99%\' )")
     DB_CONN.do("delete from ocustomer where ID_CUSTOMER in ( select ID_CUST_FK from ocard_alias where ID_CUST_FK like \'99%\' )")
     DB_CONN.do("delete from ocard_alias where ID_CUST_FK like \'99%\'")
     DB_CONN.commit
   end

   def closeStatements()
      @statementLoyFindValidCardAndHashFromOcustMktg.finish
      @statementLoyFindExpiredCardAndHashFromOcustMktg.finish
      @statementLoyFindCardNotExistingInOcustMktg.finish
   end

   def execStmt( pStmt, params )

   end

   def oacmCountById()
     return funcLoyFindCardAndHashFromOcustMktg @statementLoyFindValidCardAndHashFromOcustMktg
   end

   def cleanAcms
     DB_CONN.do("DELETE FROM OACM")
     DB_CONN.commit
   end

   def cleanAcmsById pAcmIds
     DB_CONN.do("DELETE FROM OACM WHERE ID_ACM IN (#{pAcmIds}) ")
     DB_CONN.commit
   end

   def close()
   end
      
end
