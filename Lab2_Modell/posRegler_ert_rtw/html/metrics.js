function CodeMetrics() {
	 this.metricsArray = {};
	 this.metricsArray.var = new Array();
	 this.metricsArray.fcn = new Array();
	 this.metricsArray.var["posRegler_P"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\posRegler_ert_rtw\\posRegler.c",
	size: 48};
	 this.metricsArray.var["posRegler_U"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\posRegler_ert_rtw\\posRegler.c",
	size: 16};
	 this.metricsArray.var["posRegler_Y"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\posRegler_ert_rtw\\posRegler.c",
	size: 16};
	 this.metricsArray.fcn["memset"] = {file: "C:\\Program Files\\MATLAB\\R2021a\\polyspace\\verifier\\cxx\\include\\include-libc\\string.h",
	stack: 0,
	stackTotal: 0};
	 this.metricsArray.fcn["posRegler_initialize"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\posRegler_ert_rtw\\posRegler.c",
	stack: 0,
	stackTotal: 0};
	 this.metricsArray.fcn["posRegler_step"] = {file: "D:\\tmp\\maowit00\\rt2-lab\\Lab2_Modell\\posRegler_ert_rtw\\posRegler.c",
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
	 this.codeMetricsSummary = '<a href="posRegler_metrics.html">Global Memory: 80(bytes) Maximum Stack: 24(bytes)</a>';
	}
CodeMetrics.instance = new CodeMetrics();
