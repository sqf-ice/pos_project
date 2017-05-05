/****************************************************************************
NAME
    IMSI_List.h

DESCRIPTION

REFERENCE

MODIFICATION SHEET:
    MODIFIED   (YYYY.MM.DD)
    LinHB     2013.02.18      - created
****************************************************************************/
#ifndef _IMSI_LIST_H
#define _IMSI_LIST_H

typedef struct _tagIMSI
{
	unsigned char Code[5 + 1];
	unsigned char Operator[20 + 1];
	//unsigned char Country[7 + 1];
}IMSI_INFO;

const IMSI_INFO IMSILIST[]=
{
	{"20201", "Cosmote"},
	{"20205", "Vodafone GR"},
	{"20210", "TELESTET"},
	{"20404", "Vodafone NL"},
	{"20408", "KPN"},
	{"20412", "O2 - NL"},
	{"20416", "Ben"},
	{"20420", "dutchtone"},
	{"20601", "Proximus"},
	{"20610", "Mobistar"},
	{"20620", "Orange"},
	{"20801", "Orange"},
	{"20810", "SFR"},
	{"20820", "BOUYGTEL"},
	{"21303", "Mobiland"},
	{"21401", "Vodafone E"},
	{"21402", "MoviStar"},
	{"21403", "AMENA"},
	{"21404", "Xfera"},
	{"21407", "MoviStar"},
	{"21601", "Pannon GSM"},
	{"21630", "Westel"},
	{"21670", "Vodafone"},
	{"21803", "ERONET"},
	{"21805", "Mobilna Srpska"},
	{"21890", "GSM BiH"},
	{"21901", "CRONET"},
	{"21910", "VIP"},
	{"22001", "MOBTEL"},
	{"22002", "ProMonte"},
	{"22003", "Telekom Srbija"},
	{"22004", "MONET"},
	{"22201", "TIM"},
	{"22210", "Vodafone IT"},
	{"22288", "WIND"},
	{"22298", "Blu SpA"},
	{"22601", "CONNEX"},
	{"22603", "Cosmorom"},
	{"22610", "dialog"},
	{"22801", "Swiss GSM"},
	{"22802", "sunrise"},
	{"22803", "Orange"},
	{"23001", "PAEGAS"},
	{"23002", "EUROTEL"},
	{"23003", "OSKAR"},
	{"23101", "GLOBTEL"},
	{"23102", "EUROTEL"},
	{"23201", "A1"},
	{"23203", "T-Mobile"},
	{"23205", "ONE"},
	{"23207", "tele.ring"},
	{"23410", "O2 - UK"},
	{"23415", "Vodafone"},
	{"23430", "T-Mobile UK"},
	{"23431", "T-Mobile UK"},
	{"23432", "T-Mobile UK"},
	{"23433", "Orange"},
	{"23450", "JT GSM"},
	{"23455", "GUERNSEY TEL"},
	{"23458", "MANX"},
	{"23801", "TDK-MOBIL"},
	{"23802", "SONOFON"},
	{"23820", "Telia"},
	{"23830", "Orange"},
	{"24001", "Telia S"},
	{"24007", "IQ"},
	{"24008", "Vodafone"},
	{"24201", "TELENOR"},
	{"24202", "NetCom"},
	{"24403", "Telia"},
	{"24405", "RADIOLINJA"},
	{"24409", "FINNET"},
	{"24412", "2G"},
	{"24414", "AMT"},
	{"24491", "SONERA"},
	{"24601", "OMNITEL"},
	{"24602", "Bite GSM"},
	{"24603", "TELE2"},
	{"24701", "LMT GSM"},
	{"24702", "BALTCOM"},
	{"24801", "EMT GSM"},
	{"24802", "RLE"},
	{"24803", "Q GSM"},
	{"25001", "MTS"},
	{"25002", "NorthWest GSM"},
	{"25003", "NCC"},
	{"25005", "SCS"},
	{"25007", "SMARTS"},
	{"25010", "DTC"},
	{"25011", "Orensot"},
	{"25012", "Far East"},
	{"25013", "Kuban GSM"},
	{"25016", "NTC"},
	{"25017", "Ermak RMS"},
	{"25028", "EXTEL"},
	{"25039", "Uraltel"},
	{"25044", "NC-GSM"},
	{"25091", "Sonic Duo"},
	{"25092", "Primtel"},
	{"25093", "JSC Telecom XXI"},
	{"25099", "Bee Line"},
	{"25501", "UMC"},
	{"25502", "WellCOM"},
	{"25503", "Kyivstar"},
	{"25505", "Golden Telecom"},
	{"25701", "VELCOM"},
	{"25901", "VOXTEL"},
	{"25902", "MOLDCELL"},
	{"26001", "Plus GSM"},
	{"26002", "Era GSM"},
	{"26003", "IDEA"},
	{"26201", "T-Mobile"},
	{"26202", "Vodafone D2"},
	{"26203", "E-Plus"},
	{"26207", "O2 - DE"},
	{"26213", "Mobilcom"},
	{"26601", "GIBTEL"},
	{"26801", "Vodafone"},
	{"26803", "OPTIMUS"},
	{"26806", "TMN"},
	{"27001", "LUXGSM"},
	{"27077", "TANGO"},
	{"27201", "Vodafone IRL"},
	{"27202", "O2 - IRL"},
	{"27203", "METEOR"},
	{"27401", "SIMINN"},
	{"27402", "TAL"},
	{"27404", "Viking"},
	{"27601", "AMC"},
	{"27602", "Vodafone"},
	{"27801", "Vodafone Malta"},
	{"27821", "go mobile"},
	{"28001", "CYTAGSM"},
	{"28201", "GEOCELL"},
	{"28202", "Magti GSM"},
	{"28203", "GEO 03"},
	{"28301", "ARMGSM"},
	{"28401", "M-TEL GSM"},
	{"28601", "TURKCELL"},
	{"28602", "TELSIM"},
	{"28603", "ARIA"},
	{"28604", "AYCELL"},
	{"28801", "FT GSM"},
	{"29001", "TELE Greenland"},
	{"29340", "SI.MOBIL"},
	{"29341", "MOBITEL"},
	{"29401", "MOBIMAK"},
	{"29501", "TELECOM"},
	{"29502", "MONTEL"},
	{"29505", "FL1"},
	{"29577", "LI TANGO"},
	{"30237", "Microcell"},
	{"30272", "Rogers AT&T"},
	{"31001", "Cellnet"},
	{"31011", "Wireless 2000"},
	{"31015", "Cingular"},
	{"31016", "T-Mobile"},
	{"31017", "Cingular"},
	{"31018", "Cingular"},
	{"31020", "T-Mobile"},
	{"31021", "T-Mobile"},
	{"31022", "T-Mobile"},
	{"31023", "T-Mobile"},
	{"31024", "T-Mobile"},
	{"31025", "T-Mobile"},
	{"31026", "T-Mobile"},
	{"31027", "T-Mobile"},
	{"31031", "T-Mobile"},
	{"31034", "WestLink"},
	{"31035", "Carolina"},
	{"31038", "AT&T Wireless"},
	{"31041", "Cingular"},
	{"31046", "TMP Corp"},
	{"31058", "T-Mobile"},
	{"31061", "Epic Touch"},
	{"31063", "AmeriLink"},
	{"31064", "Einstein PCS"},
	{"31066", "T-Mobile"},
	{"31067", "Wireless 2000"},
	{"31068", "NPI Wireless"},
	{"31069", "Conestoga"},
	{"31074", "Telemetrix"},
	{"31076", "PTSI"},
	{"31077", "Iowa Wireless"},
	{"31078", "Airlink PCS"},
	{"31079", "PinPoint"},
	{"31080", "T-Mobile"},
	{"31098", "AWS"},
	{"31114", "Sprocket"},
	{"31601", "Nextel"},
	{"33805", "Digicel"},
	{"34001", "Orange "},
	{"34020", "Bouygues"},
	{"34430", "APUA PCS"},
	{"35001", "Telecom"},
	{"36251", "Telcell GSM"},
	{"36801", "C_Com"},
	{"37001", "Orange"},
	{"40001", "AZERCELL GSM"},
	{"40002", "BAKCELL GSM 2000"},
	{"40101", "K-MOBILE"},
	{"40102", "K'CELL"},
	{"40401", "Hutch"},
	{"40402", "Airtel"},
	{"40403", "Airtel"},
	{"40404", "IDEA"},
	{"40405", "Hutch"},
	{"40407", "IDEA"},
	{"40409", "Reliance"},
	{"40410", "Airtel"},
	{"40411", "Hutch"},
	{"40412", "IDEA"},
	{"40413", "Hutch"},
	{"40414", "Spice"},
	{"40415", "Hutch"},
	{"40416", "Airtel"},
	{"40418", "Reliance"},
	{"40419", "IDEA"},
	{"40420", "Hutch"},
	{"40421", "BPL Mobile"},
	{"40422", "IDEA"},
	{"40424", "IDEA"},
	{"40427", "Hutch"},
	{"40429", "Aircel"},
	{"40430", "Hutch"},
	{"40431", "Airtel"},
	{"40434", "Cellone"},
	{"40436", "Reliance"},
	{"40437", "Aircel"},
	{"40438", "Cellone"},
	{"40440", "Airtel"},
	{"40441", "RPG"},
	{"40443", "Hutch"},
	{"40444", "Spice"},
	{"40445", "Airtel"},
	{"40446", "Hutch"},
	{"40449", "Airtel"},
	{"40450", "Reliance"},
	{"40451", "Cellone"},
	{"40452", "Reliance"},
	{"40453", "Cellone"},
	{"40454", "Cellone"},
	{"40455", "Cellone"},
	{"40456", "IDEA"},
	{"40457", "Cellone"},
	{"40458", "Cellone"},
	{"40459", "Cellone"},
	{"40460", "Hutch"},
	{"40462", "Cellone"},
	{"40464", "Cellone"},
	{"40466", "Cellone"},
	{"40467", "Reliance"},
	{"40468", "DOLPHIN"},
	{"40469", "MTNL"},
	{"40470", "Airtel"},
	{"40471", "Cellone"},
	{"40472", "Cellone"},
	{"40473", "Cellone"},
	{"40474", "Cellone"},
	{"40475", "Cellone"},
	{"40476", "Cellone"},
	{"40477", "Cellone"},
	{"40478", "IDEA"},
	{"40479", "Cellone"},
	{"40480", "Cellone"},
	{"40481", "Cellone"},
	{"40482", "IDEA"},
	{"40483", "Reliance"},
	{"40484", "Hutch"},
	{"40485", "Reliance"},
	{"40486", "Hutch"},
	{"40487", "IDEA"},
	{"40488", "Hutch"},
	{"40489", "IDEA"},
	{"40490", "Airtel"},
	{"40492", "Airtel"},
	{"40493", "Airtel"},
	{"40494", "Airtel"},
	{"40495", "Airtel"},
	{"40496", "Airtel"},
	{"40497", "Airtel"},
	{"40498", "Airtel"},
	{"40551", "Airtel"},
	{"40552", "Airtel"},
	{"40553", "Airtel"},
	{"40554", "Airtel"},
	{"40555", "Airtel"},
	{"40556", "Airtel"},
	{"40566", "Hutch"},
	{"40567", "Hutch"},
	{"41001", "Mobilink"},
	{"41302", "DIALOG"},
	{"41303", "CELLTEL"},
	{"41401", "MM 900"},
	{"41501", "Cellis"},
	{"41503", "LibanCell"},
	{"41601", "Fastlink"},
	{"41677", "MobileCom"},
	{"41702", "Spacetel"},
	{"41709", "MOBILE SYRIA"},
	{"41902", "MTCNet"},
	{"41903", "WATANIYA"},
	{"42001", "Al-Jawal"},
	{"42007", "EAE"},
	{"42102", "Spacetel"},
	{"42202", "OMAN MOBILE"},
	{"42402", "ETISALAT"},
	{"42501", "Orange"},
	{"42505", "JAWWAL"},
	{"42601", "MOBILE PLUS"},
	{"42701", "QATARNET"},
	{"42899", "MobiCom"},
	{"42901", "NTC"},
	{"43211", "TCI"},
	{"43214", "KISH"},
	{"43401", "Buztel"},
	{"43402", "Uzmacom"},
	{"43404", "UZB DAEWOO-GSM"},
	{"43405", "Coscom"},
	{"43701", "BITEL"},
	{"43801", "BCTI"},
	{"45201", "MOBIFONE"},
	{"45202", "VINAFONE"},
	{"45400", "CSL"},
	{"45401", "NEW WORLD"},
	{"45402", "CSL"},
	{"45404", "Orange"},
	{"45406", "SMC"},
	{"45410", "NEW WORLD"},
	{"45412", "PEOPLES"},
	{"45416", "SUNDAY"},
	{"45418", "HK TELECOM"},
	{"45500", "SmarTone"},
	{"45501", "CTMGSM"},
	{"45503", "HT Macau"},
	{"45601", "MobiTel"},
	{"45602", "SAMART-GSM"},
	{"45701", "LAO GSM"},
	{"46000", "CHINA MOBILE"},
	{"46001", "CHN-CUGSM"},
	{"46002", "CHINA MOBILE"},
	{"46601", "Far EasTone"},
	{"46606", "TUNTEX"},
	{"46668", "ACeS"},
	{"46688", "KGT"},
	{"46692", "Chunghwa"},
	{"46693", "MobiTai"},
	{"46697", "TWN GSM"},
	{"46699", "TransAsia"},
	{"47001", "GrameemPhone"},
	{"47002", "AKTEL"},
	{"47003", "ShebaWorld"},
	{"47019", "Mobile 2000"},
	{"47201", "DHIMOBILE"},
	{"50212", "Maxis Mobile"},
	{"50213", "TM Touch"},
	{"50216", "DiGi"},
	{"50217", "ADAM"},
	{"50219", "CELCOM"},
	{"50501", "MobileNet"},
	{"50502", "OPTUS"},
	{"50503", "Vodafone"},
	{"50508", "One.Tel"},
	{"51000", "ACeS"},
	{"51001", "SATELINDOCEL"},
	{"51008", "LIPPO TEL"},
	{"51010", "TELKOMSEL"},
	{"51011", "GSM-XL"},
	{"51021", "INDOSAT"},
	{"51501", "ISLACOM"},
	{"51502", "Globe"},
	{"51503", "SMART"},
	{"51505", "Digitel"},
	{"51511", "ACeS"},
	{"52001", "AIS GSM"},
	{"52015", "ACT Mobile"},
	{"52018", "WP-1800"},
	{"52020", "ACeS"},
	{"52023", "HELLO"},
	{"52099", "Orange"},
	{"52501", "ST-GSM-SGP"},
	{"52502", "ST-GSM1800-SGP"},
	{"52503", "M1-GSM-SGP"},
	{"52504", "SGP-M1-3GSM"},
	{"52505", "STARHUB-SGP"},
	{"52811", "BRU TSTCom"},
	{"53001", "Vodafone"},
	{"53901", "Tonga Comm."},
	{"54100", "ACeS"},
	{"54101", "SMILE"},
	{"54201", "Vodafone"},
	{"54411", "Blue Sky"},
	{"54601", "MOBILIS"},
	{"54720", "VINI"},
	{"55001", "FSM"},
	{"60201", "MobiNiL"},
	{"60202", "CLICK GSM"},
	{"60301", "AMN"},
	{"60400", "Meditel"},
	{"60401", "IAM"},
	{"60502", "TUNICELL"},
	{"60801", "ALIZE"},
	{"60802", "SENTEL"},
	{"61001", "MALITEL"},
	{"61101", "MOBILIS"},
	{"61102", "LAGUI"},
	{"61201", "CORA"},
	{"61203", "Ivoiris"},
	{"61205", "TELECEL"},
	{"61302", "CELTEL"},
	{"61402", "CELTEL"},
	{"61501", "TOGOCEL"},
	{"61601", "LIBERCOM"},
	{"61602", "Telecel Benin"},
	{"61603", "BENINCELL"},
	{"61701", "CELLPLUS"},
	{"61710", "EMTEL"},
	{"61801", "Omega"},
	{"62001", "SPACEFON"},
	{"62002", "ONEtouch"},
	{"62003", "MOBITEL"},
	{"62100", "MTN"},
	{"62120", "ECONET"},
	{"62130", "MTN"},
	{"62140", "NITEL GSM"},
	{"62201", "CELTEL"},
	{"62202", "LIBERTIS"},
	{"62401", "MTN-CAM"},
	{"62402", "MOBILIS"},
	{"62501", "CPV MOVEL"},
	{"62801", "LIBERTIS"},
	{"62802", "GO Celtel"},
	{"62803", "CELTEL"},
	{"62901", "CELTEL"},
	{"62910", "LIBERTIS"},
	{"63001", "CELLNET"},
	{"63002", "CELTEL"},
	{"63004", "CELLCO"},
	{"63089", "OASIS"},
	{"63301", "SEYCEL"},
	{"63310", "AIRTEL"},
	{"63401", "MobiTel"},
	{"63510", "Rwandacell"},
	{"63601", "ETMTN"},
	{"63701", "BARAKAAT"},
	{"63902", "Safaricom"},
	{"63903", "KENCELL"},
	{"64001", "TRITEL"},
	{"64002", "MobiTel"},
	{"64003", "ZANTEL"},
	{"64004", "Vodacom"},
	{"64005", "CELTEL"},
	{"64101", "CelTel"},
	{"64110", "MTN-UGANDA"},
	{"64111", "UTL TELECEL"},
	{"64201", "Spacetel"},
	{"64202", "SAFARIS"},
	{"64301", "mCel"},
	{"64501", "ZAMCELL"},
	{"64502", "TELECEL"},
	{"64601", "Madacom"},
	{"64602", "ANTARIS"},
	{"64700", "Orange Reunion"},
	{"64710", "SFR Reunion"},
	{"64801", "NET*ONE"},
	{"64803", "TELECEL"},
	{"64804", "ECONET"},
	{"64901", "MTC"},
	{"65001", "CALLPOINT 90"},
	{"65010", "CelTel"},
	{"65101", "Vodacom"},
	{"65201", "MASCOM"},
	{"65202", "VISTA"},
	{"65310", "SwaziMTN"},
	{"65501", "Vodacom"},
	{"65507", "Cell C (Pty) Ltd"},
	{"65510", "MTN"},
	{"70601", "PERSONAL"},
	{"70602", "DIGICEL"},
	{"71610", "TIM"},
	{"72207", "UNIFON"},
	{"72234", "Telecom Personal"},
	{"72235", "PORT-HABLE"},
	{"72402", "TIM BRASIL"},
	{"72403", "TIM BRASIL"},
	{"72404", "TIM BRASIL"},
	{"72405", "Claro"},
	{"72416", "BrTCel"},
	{"73001", "ENTEL PCS"},
	{"73010", "ENTEL PCS"},
	{"73401", "INFONET"},
	{"73402", "DIGITEL"},
	{"73601", "NUEVATEL"},
	{"73602", "ENTEL"},
	{"74401", "VOX"},
	{"74402", "PY 02"},
	{"74601", "ICMS"},
	{"90105", "Thuraya"},
};

#endif