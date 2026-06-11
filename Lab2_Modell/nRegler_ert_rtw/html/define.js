function CodeDefine() { 
this.def = new Array();
this.def["nRegler_P"] = {file: "nRegler_c.html",line:23,type:"var"};
this.def["nRegler_DW"] = {file: "nRegler_c.html",line:86,type:"var"};
this.def["nRegler_U"] = {file: "nRegler_c.html",line:89,type:"var"};
this.def["nRegler_Y"] = {file: "nRegler_c.html",line:92,type:"var"};
this.def["nRegler_step"] = {file: "nRegler_c.html",line:95,type:"fcn"};
this.def["nRegler_initialize"] = {file: "nRegler_c.html",line:165,type:"fcn"};
this.def["DW_nRegler_T"] = {file: "nRegler_h.html",line:39,type:"type"};
this.def["ExtU_nRegler_T"] = {file: "nRegler_h.html",line:47,type:"type"};
this.def["ExtY_nRegler_T"] = {file: "nRegler_h.html",line:55,type:"type"};
this.def["P_nRegler_T"] = {file: "nRegler_h.html",line:96,type:"type"};
this.def["int8_T"] = {file: "rtwtypes_h.html",line:51,type:"type"};
this.def["uint8_T"] = {file: "rtwtypes_h.html",line:52,type:"type"};
this.def["int16_T"] = {file: "rtwtypes_h.html",line:53,type:"type"};
this.def["uint16_T"] = {file: "rtwtypes_h.html",line:54,type:"type"};
this.def["int32_T"] = {file: "rtwtypes_h.html",line:55,type:"type"};
this.def["uint32_T"] = {file: "rtwtypes_h.html",line:56,type:"type"};
this.def["real32_T"] = {file: "rtwtypes_h.html",line:57,type:"type"};
this.def["real64_T"] = {file: "rtwtypes_h.html",line:58,type:"type"};
this.def["real_T"] = {file: "rtwtypes_h.html",line:64,type:"type"};
this.def["time_T"] = {file: "rtwtypes_h.html",line:65,type:"type"};
this.def["boolean_T"] = {file: "rtwtypes_h.html",line:66,type:"type"};
this.def["int_T"] = {file: "rtwtypes_h.html",line:67,type:"type"};
this.def["uint_T"] = {file: "rtwtypes_h.html",line:68,type:"type"};
this.def["ulong_T"] = {file: "rtwtypes_h.html",line:69,type:"type"};
this.def["char_T"] = {file: "rtwtypes_h.html",line:70,type:"type"};
this.def["uchar_T"] = {file: "rtwtypes_h.html",line:71,type:"type"};
this.def["byte_T"] = {file: "rtwtypes_h.html",line:72,type:"type"};
this.def["pointer_T"] = {file: "rtwtypes_h.html",line:90,type:"type"};
}
CodeDefine.instance = new CodeDefine();
var testHarnessInfo = {OwnerFileName: "", HarnessOwner: "", HarnessName: "", IsTestHarness: "0"};
var relPathToBuildDir = "../ert_main.c";
var fileSep = "\\";
var isPC = true;
function Html2SrcLink() {
	this.html2SrcPath = new Array;
	this.html2Root = new Array;
	this.html2SrcPath["nRegler_c.html"] = "../nRegler.c";
	this.html2Root["nRegler_c.html"] = "nRegler_c.html";
	this.html2SrcPath["nRegler_h.html"] = "../nRegler.h";
	this.html2Root["nRegler_h.html"] = "nRegler_h.html";
	this.html2SrcPath["rtwtypes_h.html"] = "../rtwtypes.h";
	this.html2Root["rtwtypes_h.html"] = "rtwtypes_h.html";
	this.html2SrcPath["rtmodel_h.html"] = "../rtmodel.h";
	this.html2Root["rtmodel_h.html"] = "rtmodel_h.html";
	this.getLink2Src = function (htmlFileName) {
		 if (this.html2SrcPath[htmlFileName])
			 return this.html2SrcPath[htmlFileName];
		 else
			 return null;
	}
	this.getLinkFromRoot = function (htmlFileName) {
		 if (this.html2Root[htmlFileName])
			 return this.html2Root[htmlFileName];
		 else
			 return null;
	}
}
Html2SrcLink.instance = new Html2SrcLink();
var fileList = [
"nRegler_c.html","nRegler_h.html","rtwtypes_h.html","rtmodel_h.html"];
