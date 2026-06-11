function CodeMetrics() {
	 this.metricsArray = {};
	 this.metricsArray.var = new Array();
	 this.metricsArray.fcn = new Array();
	 this.metricsArray.var["nRegler_DW"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	size: 16};
	 this.metricsArray.var["nRegler_P"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	size: 104};
	 this.metricsArray.var["nRegler_U"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	size: 16};
	 this.metricsArray.var["nRegler_Y"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	size: 16};
	 this.metricsArray.fcn["memset"] = {file: "C:\\Program Files\\MATLAB\\R2021a\\polyspace\\verifier\\cxx\\include\\include-libc\\string.h",
	stack: 0,
	stackTotal: 0};
	 this.metricsArray.fcn["nRegler_initialize"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	stack: 0,
	stackTotal: 0};
	 this.metricsArray.fcn["nRegler_step"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\nRegler_ert_rtw\\nRegler.c",
	stack: 24,
	stackTotal: 24};
	 this.getMetrics = function(token) { 
		 var data;
		 data = this.metricsArray.var[token];
		 if (!data) {
			 data = this.metricsArray.fcn[token];
			 if (data) data.type = "fcn";
		 } else { 
			 data.type = "var";
		 }
	 return data; }; 
	 this.codeMetricsSummary = '<a href="nRegler_metrics.html">Global Memory: 152(bytes) Maximum Stack: 24(bytes)</a>';
	}
CodeMetrics.instance = new CodeMetrics();
