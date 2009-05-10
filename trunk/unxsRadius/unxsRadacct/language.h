// $Id: language.h,v 1.9 2002/11/29 14:39:00 ggw Exp $
// English: USA
// Navigation bar buttons and results

#define LANG_NB_LIST "List"
#define LANG_NB_MODIFY "Modify"
#define LANG_NB_CONFIRMMOD "Confirm Modify"
#define LANG_NB_NEW "New"
#define LANG_NB_DELETE "Delete"
#define LANG_NB_CONFIRMNEW "Create New Record"
#define LANG_NBB_CONFIRMNEW "<input class=lrevButton title='Double check!' type=submit name=gcCommand value=\"Create New Record\">"
#define LANG_NB_CONFIRMDEL "Confirm Delete"
#define LANG_NBB_CONFIRMDEL "<input class=lwarnButton title='No undo available!' type=submit name=gcCommand value=\"Confirm Delete\">"
#define LANG_NBB_CONFIRMMOD "<input class=lalertButton title='Double check!' type=submit name=gcCommand value=\"Confirm Modify\">"


#define LANG_NBR_FOUND " Found"
#define LANG_NBR_MODIFIED " Modified"
#define LANG_NBR_NEW " New"
#define LANG_NBR_NORECS " No records found"
#define LANG_NBRF_SHOWING " (Showing %lu of %d)"

#define LANG_NBR_RECEXISTS "<blink>Record already exists"
#define LANG_NBR_NEWRECADDED "New record %u added"
#define LANG_NBR_RECDELETED "Record Deleted"
#define LANG_NBR_RECNOTDELETED "Record Not Deleted"
#define LANG_NBR_RECNOTEXIST "<blink>Record does not exist"
#define LANG_NBR_MULTRECS "<blink>Multiple rows!"
#define LANG_NBR_EXTMOD "<blink>This record was just modified by another user!"
#define LANG_NBRF_REC_MODIFIED "record %s modified"

#define LANG_NBB_SKIPFIRST "<input style='width:20px;' title=\"Skip to first record\" type=submit name=gcFind value=\"<<\" >"
#define LANG_NBB_SKIPBACK "<input style='width:20px;' title=\"Go back one record\" type=submit name=gcFind value=\"< \" >"
#define LANG_NBB_SKIPNEXT "<input style='width:20px;' title=\"Advance to next record\" type=submit name=gcFind value=\" >\" >"
#define LANG_NBB_SKIPLAST "<input style='width:20px;' title=\"Skip to last record\" type=submit name=gcFind value=\">>\" >\n"

//Modified for local .h use
#define LANG_NBB_NEWREV "<input class=revButton title=\"Create new record\" type=submit name=gcCommand value=New>"
#define LANG_NBB_NEW "<input  title=\"Create new record\" type=submit name=gcCommand value=New>"
#define LANG_NBB_DELETE "<input class=warnButton title=\"Delete current record\" type=submit name=gcCommand value=Delete>"
#define LANG_NBB_MODIFY "<input class=alertButton title=\"Modify current record\" type=submit name=gcCommand value=Modify>"

#define LANG_NBB_LIST "<input title=\"Paginated listing of all records\" type=submit name=gcFind value=List>"
#define LANG_NBB_SEARCH "<input class=revButton title=\"Find record with parameters set below\" type=submit name=gcFind value=Search>"

#define LANG_PAGEMACHINE_HINT "</center><table bgcolor=black><tr><td><font face=arial,helvetica><font color=white> No matching records found.<br><font size=1 color=red>-->Hint:</font><font size=1> Use %% wildcard. The %% is equal to one or more characters.</table>"
#define LANG_PAGEMACHINE_SHOWING "Page %d/%d (Records %d-%d of %d)\n"
#define LANG_PAGEMACHINE_SEARCH "<input type=submit name=gcFind value=Search >"
#define LANG_PAGEMACHINE_SEARCHBUTTON "Search"

//tRadacct
#define LANG_FL_tRadacct_uRadacct "uRadacct"
#define LANG_FT_tRadacct_uRadacct "Primary key for interface use only"
#define LANG_FL_tRadacct_cLogin "cLogin"
#define LANG_FT_tRadacct_cLogin "Login name"
#define LANG_FL_tRadacct_cSessionID "cSessionID"
#define LANG_FT_tRadacct_cSessionID "Radius session ID"
#define LANG_FL_tRadacct_cNAS "cNAS"
#define LANG_FT_tRadacct_cNAS "NAS server IP"
#define LANG_FL_tRadacct_uConnectTime "uConnectTime"
#define LANG_FT_tRadacct_uConnectTime "Seconds from stop record"
#define LANG_FL_tRadacct_uPort "uPort"
#define LANG_FT_tRadacct_uPort "NAS port used during session"
#define LANG_FL_tRadacct_uPortType "uPortType"
#define LANG_FT_tRadacct_uPortType "NAS port_type"
#define LANG_FL_tRadacct_uService "uService"
#define LANG_FT_tRadacct_uService "Service type PPP/Telnet/Etc"
#define LANG_FL_tRadacct_uProtocol "uProtocol"
#define LANG_FT_tRadacct_uProtocol "Session Protocol"
#define LANG_FL_tRadacct_cIP "cIP"
#define LANG_FT_tRadacct_cIP "IP assigned"
#define LANG_FL_tRadacct_cLine "cLine"
#define LANG_FT_tRadacct_cLine "PoP number called"
#define LANG_FL_tRadacct_cCallerID "cCallerID"
#define LANG_FT_tRadacct_cCallerID "Caller ID"
#define LANG_FL_tRadacct_uInBytes "uInBytes"
#define LANG_FT_tRadacct_uInBytes "Bytes from user"
#define LANG_FL_tRadacct_uOutBytes "uOutBytes"
#define LANG_FT_tRadacct_uOutBytes "Bytes to user"
#define LANG_FL_tRadacct_uTermCause "uTermCause"
#define LANG_FT_tRadacct_uTermCause "Termination cause code"
#define LANG_FL_tRadacct_uStartTime "uStartTime"
#define LANG_FT_tRadacct_uStartTime "Start record time"
#define LANG_FL_tRadacct_uStopTime "uStopTime"
#define LANG_FT_tRadacct_uStopTime "Stop record time"
#define LANG_FL_tRadacct_cInfo "cInfo"
#define LANG_FT_tRadacct_cInfo "Connect info"
//tMonth
#define LANG_FL_tMonth_uMonth "uMonth"
#define LANG_FT_tMonth_uMonth "Primary Key"
#define LANG_FL_tMonth_cLabel "cLabel"
#define LANG_FT_tMonth_cLabel "Short label"
#define LANG_FL_tMonth_uOwner "uOwner"
#define LANG_FT_tMonth_uOwner "Record owner"
#define LANG_FL_tMonth_uCreatedBy "uCreatedBy"
#define LANG_FT_tMonth_uCreatedBy "uClient for last insert"
#define LANG_FL_tMonth_uCreatedDate "uCreatedDate"
#define LANG_FT_tMonth_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tMonth_uModBy "uModBy"
#define LANG_FT_tMonth_uModBy "uClient for last update"
#define LANG_FL_tMonth_uModDate "uModDate"
#define LANG_FT_tMonth_uModDate "Unix seconds date last update"
//tRadacctMonth
#define LANG_FL_tRadacctMonth_uRadacct "uRadacct"
#define LANG_FT_tRadacctMonth_uRadacct "Primary key for interface use only"
#define LANG_FL_tRadacctMonth_cLogin "cLogin"
#define LANG_FT_tRadacctMonth_cLogin "Login name"
#define LANG_FL_tRadacctMonth_cSessionID "cSessionID"
#define LANG_FT_tRadacctMonth_cSessionID "Radius session ID"
#define LANG_FL_tRadacctMonth_cNAS "cNAS"
#define LANG_FT_tRadacctMonth_cNAS "NAS server IP"
#define LANG_FL_tRadacctMonth_uConnectTime "uConnectTime"
#define LANG_FT_tRadacctMonth_uConnectTime "Seconds from stop record"
#define LANG_FL_tRadacctMonth_uPort "uPort"
#define LANG_FT_tRadacctMonth_uPort "NAS port used during session"
#define LANG_FL_tRadacctMonth_uPortType "uPortType"
#define LANG_FT_tRadacctMonth_uPortType "NAS port_type"
#define LANG_FL_tRadacctMonth_uService "uService"
#define LANG_FT_tRadacctMonth_uService "Service type PPP/Telnet/Etc"
#define LANG_FL_tRadacctMonth_uProtocol "uProtocol"
#define LANG_FT_tRadacctMonth_uProtocol "Session Protocol"
#define LANG_FL_tRadacctMonth_cIP "cIP"
#define LANG_FT_tRadacctMonth_cIP "IP assigned"
#define LANG_FL_tRadacctMonth_cLine "cLine"
#define LANG_FT_tRadacctMonth_cLine "PoP number called"
#define LANG_FL_tRadacctMonth_cCallerID "cCallerID"
#define LANG_FT_tRadacctMonth_cCallerID "Caller ID"
#define LANG_FL_tRadacctMonth_uInBytes "uInBytes"
#define LANG_FT_tRadacctMonth_uInBytes "Bytes from user"
#define LANG_FL_tRadacctMonth_uOutBytes "uOutBytes"
#define LANG_FT_tRadacctMonth_uOutBytes "Bytes to user"
#define LANG_FL_tRadacctMonth_uTermCause "uTermCause"
#define LANG_FT_tRadacctMonth_uTermCause "Termination cause code"
#define LANG_FL_tRadacctMonth_uStartTime "uStartTime"
#define LANG_FT_tRadacctMonth_uStartTime "Start record time"
#define LANG_FL_tRadacctMonth_uStopTime "uStopTime"
#define LANG_FT_tRadacctMonth_uStopTime "Stop record time"
#define LANG_FL_tRadacctMonth_cInfo "cInfo"
#define LANG_FT_tRadacctMonth_cInfo "Connect info"
//tConfiguration
#define LANG_FL_tConfiguration_uConfiguration "uConfiguration"
#define LANG_FT_tConfiguration_uConfiguration "Primary Key"
#define LANG_FL_tConfiguration_uServer "uServer"
#define LANG_FL_tConfiguration_cLabel "cLabel"
#define LANG_FT_tConfiguration_cLabel "Short label"
#define LANG_FL_tConfiguration_cValue "cValue"
#define LANG_FT_tConfiguration_cValue "Value of name(cLabel)/value pair"
#define LANG_FL_tConfiguration_cComment "cComment"
#define LANG_FT_tConfiguration_cComment "Comment about this configuration name/value pair"
#define LANG_FL_tConfiguration_uOwner "uOwner"
#define LANG_FT_tConfiguration_uOwner "Record owner"
#define LANG_FL_tConfiguration_uCreatedBy "uCreatedBy"
#define LANG_FT_tConfiguration_uCreatedBy "uClient for last insert"
#define LANG_FL_tConfiguration_uCreatedDate "uCreatedDate"
#define LANG_FT_tConfiguration_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tConfiguration_uModBy "uModBy"
#define LANG_FT_tConfiguration_uModBy "uClient for last update"
#define LANG_FL_tConfiguration_uModDate "uModDate"
#define LANG_FT_tConfiguration_uModDate "Unix seconds date last update"
//tAuthorize
#define LANG_FL_tAuthorize_uAuthorize "uAuthorize"
#define LANG_FT_tAuthorize_uAuthorize "Primary Key"
#define LANG_FL_tAuthorize_cLabel "cLabel"
#define LANG_FT_tAuthorize_cLabel "Short label"
#define LANG_FL_tAuthorize_cIpMask "cIpMask"
#define LANG_FT_tAuthorize_cIpMask "Allow user from this IP"
#define LANG_FL_tAuthorize_uPerm "uPerm"
#define LANG_FT_tAuthorize_uPerm "User permission level"
#define LANG_FL_tAuthorize_uCertClient "uCertClient"
#define LANG_FT_tAuthorize_uCertClient "User uClient"
#define LANG_FL_tAuthorize_cPasswd "cPasswd"
#define LANG_FT_tAuthorize_cPasswd "Unix crypt() password"
#define LANG_FL_tAuthorize_cClrPasswd "cClrPasswd"
#define LANG_FT_tAuthorize_cClrPasswd "Optionally used non encrypted login password"
#define LANG_FL_tAuthorize_uOwner "uOwner"
#define LANG_FT_tAuthorize_uOwner "Record owner"
#define LANG_FL_tAuthorize_uCreatedBy "uCreatedBy"
#define LANG_FT_tAuthorize_uCreatedBy "uClient for last insert"
#define LANG_FL_tAuthorize_uCreatedDate "uCreatedDate"
#define LANG_FT_tAuthorize_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tAuthorize_uModBy "uModBy"
#define LANG_FT_tAuthorize_uModBy "uClient for last update"
#define LANG_FL_tAuthorize_uModDate "uModDate"
#define LANG_FT_tAuthorize_uModDate "Unix seconds date last update"
//tClient
#define LANG_FL_tClient_uClient "uClient"
#define LANG_FT_tClient_uClient "Primary Key"
#define LANG_FL_tClient_cLabel "cLabel"
#define LANG_FT_tClient_cLabel "Short label"
#define LANG_FL_tClient_cInfo "cInfo"
#define LANG_FT_tClient_cInfo "Unformatted info/address etc."
#define LANG_FL_tClient_cEmail "cEmail"
#define LANG_FT_tClient_cEmail "Main Email"
#define LANG_FL_tClient_cCode "cCode"
#define LANG_FT_tClient_cCode "Enterprise wide customer/contact tracking"
#define LANG_FL_tClient_uOwner "uOwner"
#define LANG_FT_tClient_uOwner "Record owner"
#define LANG_FL_tClient_uCreatedBy "uCreatedBy"
#define LANG_FT_tClient_uCreatedBy "uClient for last insert"
#define LANG_FL_tClient_uCreatedDate "uCreatedDate"
#define LANG_FT_tClient_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tClient_uModBy "uModBy"
#define LANG_FT_tClient_uModBy "uClient for last update"
#define LANG_FL_tClient_uModDate "uModDate"
#define LANG_FT_tClient_uModDate "Unix seconds date last update"
//tLog
#define LANG_FL_tLog_uLog "uLog"
#define LANG_FT_tLog_uLog "Primary Key"
#define LANG_FL_tLog_cLabel "cLabel"
#define LANG_FT_tLog_cLabel "Short label"
#define LANG_FL_tLog_uLogType "uLogType"
#define LANG_FT_tLog_uLogType "Log Type"
#define LANG_FL_tLog_cHash "cHash"
#define LANG_FT_tLog_cHash "Security hash to complicate tampering"
#define LANG_FL_tLog_uPermLevel "uPermLevel"
#define LANG_FT_tLog_uPermLevel "User Perm Level"
#define LANG_FL_tLog_uLoginClient "uLoginClient"
#define LANG_FT_tLog_uLoginClient "Client Number"
#define LANG_FL_tLog_cLogin "cLogin"
#define LANG_FT_tLog_cLogin "Login name"
#define LANG_FL_tLog_cHost "cHost"
#define LANG_FT_tLog_cHost "Ip Address"
#define LANG_FL_tLog_uTablePK "uTablePK"
#define LANG_FT_tLog_uTablePK "Primar Key of the Table"
#define LANG_FL_tLog_cTableName "cTableName"
#define LANG_FT_tLog_cTableName "Name of the Table"
#define LANG_FL_tLog_cMessage "cMessage"
#define LANG_FT_tLog_cMessage "Message text"
#define LANG_FL_tLog_cServer "cServer"
#define LANG_FT_tLog_cServer "Server name"
#define LANG_FL_tLog_uOwner "uOwner"
#define LANG_FT_tLog_uOwner "Record owner"
#define LANG_FL_tLog_uCreatedBy "uCreatedBy"
#define LANG_FT_tLog_uCreatedBy "uClient for last insert"
#define LANG_FL_tLog_uCreatedDate "uCreatedDate"
#define LANG_FT_tLog_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tLog_uModBy "uModBy"
#define LANG_FT_tLog_uModBy "uClient for last update"
#define LANG_FL_tLog_uModDate "uModDate"
#define LANG_FT_tLog_uModDate "Unix seconds date last update"
//tLogType
#define LANG_FL_tLogType_uLogType "uLogType"
#define LANG_FT_tLogType_uLogType "Primary Key"
#define LANG_FL_tLogType_cLabel "cLabel"
#define LANG_FT_tLogType_cLabel "Short label"
#define LANG_FL_tLogType_uOwner "uOwner"
#define LANG_FT_tLogType_uOwner "Record owner"
#define LANG_FL_tLogType_uCreatedBy "uCreatedBy"
#define LANG_FT_tLogType_uCreatedBy "uClient for last insert"
#define LANG_FL_tLogType_uCreatedDate "uCreatedDate"
#define LANG_FT_tLogType_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tLogType_uModBy "uModBy"
#define LANG_FT_tLogType_uModBy "uClient for last update"
#define LANG_FL_tLogType_uModDate "uModDate"
#define LANG_FT_tLogType_uModDate "Unix seconds date last update"
//tLogMonth
#define LANG_FL_tLogMonth_uLog "uLog"
#define LANG_FT_tLogMonth_uLog "Primary Key"
#define LANG_FL_tLogMonth_cLabel "cLabel"
#define LANG_FT_tLogMonth_cLabel "Short label"
#define LANG_FL_tLogMonth_uLogType "uLogType"
#define LANG_FT_tLogMonth_uLogType "Log Type"
#define LANG_FL_tLogMonth_cHash "cHash"
#define LANG_FT_tLogMonth_cHash "Security hash to complicate tampering"
#define LANG_FL_tLogMonth_uPermLevel "uPermLevel"
#define LANG_FT_tLogMonth_uPermLevel "User Perm Level"
#define LANG_FL_tLogMonth_uLoginClient "uLoginClient"
#define LANG_FT_tLogMonth_uLoginClient "Client Number"
#define LANG_FL_tLogMonth_cLogin "cLogin"
#define LANG_FT_tLogMonth_cLogin "Login name"
#define LANG_FL_tLogMonth_cHost "cHost"
#define LANG_FT_tLogMonth_cHost "Ip Address"
#define LANG_FL_tLogMonth_uTablePK "uTablePK"
#define LANG_FT_tLogMonth_uTablePK "Primar Key of the Table"
#define LANG_FL_tLogMonth_cTableName "cTableName"
#define LANG_FT_tLogMonth_cTableName "Name of the Table"
#define LANG_FL_tLogMonth_uOwner "uOwner"
#define LANG_FT_tLogMonth_uOwner "Record owner"
#define LANG_FL_tLogMonth_uCreatedBy "uCreatedBy"
#define LANG_FT_tLogMonth_uCreatedBy "uClient for last insert"
#define LANG_FL_tLogMonth_uCreatedDate "uCreatedDate"
#define LANG_FT_tLogMonth_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tLogMonth_uModBy "uModBy"
#define LANG_FT_tLogMonth_uModBy "uClient for last update"
#define LANG_FL_tLogMonth_uModDate "uModDate"
#define LANG_FT_tLogMonth_uModDate "Unix seconds date last update"
//tGlossary
#define LANG_FL_tGlossary_uGlossary "uGlossary"
#define LANG_FT_tGlossary_uGlossary "Primary Key"
#define LANG_FL_tGlossary_cLabel "cLabel"
#define LANG_FT_tGlossary_cLabel "Short label"
#define LANG_FL_tGlossary_cText "cText"
#define LANG_FT_tGlossary_cText "Definition of the label referenced in cLabel"
#define LANG_FL_tGlossary_uOwner "uOwner"
#define LANG_FT_tGlossary_uOwner "Record owner"
#define LANG_FL_tGlossary_uCreatedBy "uCreatedBy"
#define LANG_FT_tGlossary_uCreatedBy "uClient for last insert"
#define LANG_FL_tGlossary_uCreatedDate "uCreatedDate"
#define LANG_FT_tGlossary_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tGlossary_uModBy "uModBy"
#define LANG_FT_tGlossary_uModBy "uClient for last update"
#define LANG_FL_tGlossary_uModDate "uModDate"
#define LANG_FT_tGlossary_uModDate "Unix seconds date last update"
//tServer
#define LANG_FL_tServer_uServer "uServer"
#define LANG_FT_tServer_uServer "Primary Key"
#define LANG_FL_tServer_cLabel "cLabel"
#define LANG_FT_tServer_cLabel "Short label"
#define LANG_FL_tServer_uOwner "uOwner"
#define LANG_FT_tServer_uOwner "Record owner"
#define LANG_FL_tServer_uCreatedBy "uCreatedBy"
#define LANG_FT_tServer_uCreatedBy "uClient for last insert"
#define LANG_FL_tServer_uCreatedDate "uCreatedDate"
#define LANG_FT_tServer_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tServer_uModBy "uModBy"
#define LANG_FT_tServer_uModBy "uClient for last update"
#define LANG_FL_tServer_uModDate "uModDate"
#define LANG_FT_tServer_uModDate "Unix seconds date last update"
//tMonthLog
#define LANG_FL_tMonthLog_uMonthLog "uMonthLog"
#define LANG_FT_tMonthLog_uMonthLog "Primary Key"
#define LANG_FL_tMonthLog_cLabel "cLabel"
#define LANG_FT_tMonthLog_cLabel "Short label"
#define LANG_FL_tMonthLog_uOwner "uOwner"
#define LANG_FT_tMonthLog_uOwner "Record owner"
#define LANG_FL_tMonthLog_uCreatedBy "uCreatedBy"
#define LANG_FT_tMonthLog_uCreatedBy "uClient for last insert"
#define LANG_FL_tMonthLog_uCreatedDate "uCreatedDate"
#define LANG_FT_tMonthLog_uCreatedDate "Unix seconds date last insert"
#define LANG_FL_tMonthLog_uModBy "uModBy"
#define LANG_FT_tMonthLog_uModBy "uClient for last update"
#define LANG_FL_tMonthLog_uModDate "uModDate"
#define LANG_FT_tMonthLog_uModDate "Unix seconds date last update"

