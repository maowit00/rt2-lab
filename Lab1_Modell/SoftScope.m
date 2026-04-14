% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Software scope to connect to Dragon12 boards via serial communication
% Displays signals channel 1 and channel 2 sent by the Dragon12 board via function SW_OSZI()
% Values are sent as 16 bit integers. Use ch1Gain, ch1Ofs, ... to rescale when display these values.
%
% Parameters:
% comIF: if comIF == 0 a list of available COM ports is printed. 
%        else comIF should be set to the serial interface of the PC to which the Dragon12 board is connected, something like 1,2,...
% ch1Gain, ch1Ofs, ch2Gain, ch2Ofs ... Gain and offset to rescale values. Default for gains: 1, for offsets: 0.
%
% When displaying values, on the horizontal axis we assume a sample time of T = 1536µs.
% The data channel 1 and channel 2 are shown in the upper plot.
% The lower plot marks data points in the upper plot which were not correctly received. In this case the value of the last valid data sample is held.

function SoftScope(comIF_x, ch1Gain_x, ch2Gain_x, ch1Ofs_x, ch2Ofs_x)
global hFig hStart hStop hGain1 hOfs1 hGain2 hOfs2 uicTitle uicYLabel runFlag comIF ch1Gain ch2Gain ch1Ofs ch2Ofs 

SOFTSCOPEVERSION = 1.95;

if     nargin < 1 && isempty(comIF)
    comIF = 2;
elseif nargin >= 1
    comIF = comIF_x;
end
if     nargin < 2 && isempty(ch1Gain)
    ch1Gain = 1.0;
elseif nargin >= 2
    ch1Gain = ch1Gain_x;
end
if     nargin < 3 && isempty(ch2Gain)
    ch2Gain = 1.0;
elseif nargin >= 3
    ch2Gain = ch2Gain_x;
end
if     nargin < 4 && isempty(ch1Ofs)
    ch1Ofs = 0;
elseif nargin >= 4
    ch1Ofs = ch1Ofs_x;
end
if     nargin < 5 && isempty(ch2Ofs)
    ch2Ofs = 0;
elseif nargin >= 5
    ch2Ofs = ch2Ofs_x;
end

CMD_STOP            = 0;
CMD_START_NOLOG     = 1;
CMD_START_WITHLOG   = 2;
CMD_READDATA        = 3;
CMD_ENUM            = 4;
CMD_VERSION         = 5;


if isunix % Linux platform
    if exist('SoftScopeDriver.mexa64','file')==0
        fprintf('ERROR 0: SoftScopeDriver.mexa64 not found - please check - Exiting\n');
        return
    end
elseif ispc % Windows platform
  if exist('SoftScopeDriver.mexw64','file')==0
      fprintf('ERROR 0: SoftScopeDriver.mexw64 not found - please check - Exiting\n');
      return
  end
else
  fprintf('ERROR 0: The current operating system is not supported - Exiting\n');
  return;
end

try
    SOFTSCOPEDRIVERVERSION = SoftScopeDriver(0,CMD_VERSION);
catch
    fprintf(1,'ERROR 9: SoftScopeDriver version check failed\n');
    ME.message
    ME.stack
end
    
if (comIF==0)
    try
        SoftScopeDriver(0,CMD_ENUM);
    catch
        fprintf(1,'ERROR 1: SoftScopeDriver serial port enumeration failed - exiting\n');
        ME.message
        ME.stack
    end
    return
end

% comIF assumed here to be \ge 1
% decrement comIF because our serial library starts numbering com ports beginning from 0; comIF=1 thus maps to interface COM2 
comIF = comIF-1; 
runFlag=1;
T = 1536e-6;                    % Dragon12 sample time (RTI time period)
timeRange=3;                    % Display 3 seconds
NSAMPLES = fix(timeRange/T)-1;  % Display time range, e.g. 3 seconds

if isempty(hFig)
    hFig=figure('CloseRequestFcn',@closereq);
    
    uicontrol('style','text','String','Ch 1 Gain',             'Unit','Normalized', 'Position',[0.01 0.93  0.07 0.05]);
    hGain1=uicontrol('style','edit','String',num2str(ch1Gain), 'Unit','Normalized', 'Position',[0.01 0.90  0.07 0.05]);
    
    uicontrol('style','text','String','Ch 2 Offset',           'Unit','Normalized', 'Position',[0.01 0.83  0.07 0.05]);
    hOfs1=uicontrol( 'style','edit','String',num2str(ch1Ofs),  'Unit','Normalized', 'Position',[0.01 0.80  0.07 0.05]);
    
    uicontrol('style','text','String','Ch 2 Gain',             'Unit','Normalized', 'Position',[0.01 0.73  0.07 0.05]);
    hGain2=uicontrol('style','edit','String',num2str(ch2Gain), 'Unit','Normalized', 'Position',[0.01 0.70  0.07 0.05]);
    
    uicontrol('style','text','String','Ch 2 Offset:',          'Unit','Normalized', 'Position',[0.01 0.63  0.07 0.05]);
    hOfs2=uicontrol( 'style','edit','String',num2str(ch2Ofs),  'Unit','Normalized', 'Position',[0.01 0.60  0.07 0.05]);

    uicontrol('style','text','String','Input Title', 'Unit','Normalized', 'Position',[0.01 0.53  0.07 0.05]);
    uicTitle=uicontrol( 'style','edit','String','dummy',  'Unit','Normalized', 'Position',[0.01 0.50  0.07 0.05]);

    uicontrol('style','text','String','Input Ylabel', 'Unit','Normalized', 'Position',[0.01 0.43  0.07 0.05]);
    uicYLabel=uicontrol( 'style','edit','String','dummy',  'Unit','Normalized', 'Position',[0.01 0.40  0.07 0.05]);

    uicontrol('style','text','String','Input Title or', 'Unit','Normalized', 'Position',[0.001 0.30  0.10 0.05]);
    uicontrol('style','text','String','Ylabel and hit', 'Unit','Normalized', 'Position',[0.001 0.27  0.10 0.05]);
    uicontrol('style','text','String','return - wait',  'Unit','Normalized', 'Position',[0.001 0.24  0.10 0.05]);
    
    hStart=uicontrol('style','pushbutton','String','Start', 'Callback', @startButtonCallback,'Visible','off','Unit','Normalized', 'Position',[0.01 0.10 0.05 0.05]);
    hStop =uicontrol('style','pushbutton','String','Stop',  'Callback', @stopButtonCallback, 'Visible','on', 'Unit','Normalized', 'Position',[0.01 0.05 0.05 0.05]);
else % happens in case the start butten is pressed (after a previous activation of the stop butten)
    ch1Gain = str2num(get(hGain1,'String'));
    ch2Gain = str2num(get(hGain2,'String'));
    ch1Ofs  = str2num(get(hOfs1, 'String'));
    ch2Ofs  = str2num(get(hOfs2, 'String'));
    titletext  = get(uicTitle,  'String');
    ylabeltext = get(uicYLabel, 'String');
end

figure(hFig)
set(hFig,'Position',[100 100, 1200, 800]);
PosSubplotUpper  = [0.15 0.15 0.83 0.8];
PosSubplotLower = [0.15 0.007 0.83 0.07];
% upper plot
subplot('Position',PosSubplotUpper);
title('Channel 1 -> red, Channel 2 -> blue');
axis([0 timeRange 0 1]);
xlabel('Time in seconds')
axU = gca;
% lower plot
subplot('Position',PosSubplotLower);
set(gca,'xtick',[]); set(gca,'xticklabel',[]); set(gca,'ytick',[]); set(gca,'yticklabel',[]);
lim = axis; lim(3) = 0; lim(4) = 1.2; axis(lim);
text(-.01,.5,'Valid?','HorizontalAlignment','right','Units','normalized')
axL = gca;
% zoom plots together with respect to x axis
linkaxes([axU axL],'x')

set(hStart,'Visible','off');
set(hStop, 'Visible','on');

fprintf(1,'SoftScope V%4.2f Driver V%4.2f started\n', SOFTSCOPEVERSION, SOFTSCOPEDRIVERVERSION);pause(1)
%try
    i   = 0;
    ch1 = [];
    ch2 = [];
    ch12_nok = [];
    try
        SoftScopeDriver(comIF, CMD_START_NOLOG);
    catch
        fprintf(1,'ERROR 2: SoftScopeDriver start command failed - exiting\n');
        ME.message
        ME.stack
        return
    end
    
    %%% Loop while running %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    while runFlag
        ch1Gain = str2num(get(hGain1,'String'));
        ch2Gain = str2num(get(hGain2,'String'));
        ch1Ofs  = str2num(get(hOfs1, 'String'));
        ch2Ofs  = str2num(get(hOfs2, 'String'));
        titletext = get(uicTitle, 'String');
        ylabeltext = get(uicYLabel, 'String');
        try
            [ch1temp,ch2temp,ch12_noktemp,n,errChk,errSn]=SoftScopeDriver(comIF, CMD_READDATA);
        catch
            fprintf(1,'ERROR 3: SoftScopeDriver read data failed - exiting\n');
            ME.message
            ME.stack
            return
        end
        ch1 = [ch1 ch1temp];
        ch2 = [ch2 ch2temp];
        ch12_nok = [ch12_nok ch12_noktemp];
        n1 = length(ch1);
        i = i + abs(n);
        if n < 0
            fprintf(1,'Read n=%8d bytes from Dragon12 - Total i=%8d bytes - Buffer overflow\n', n, i);
        else
            fprintf(1,'Read n=%8d bytes from Dragon12 - Total i=%8d bytes (%u/%u)\n', n, i,errChk, errSn);
        end

        if n1 > NSAMPLES
            ch1     =ch1(n1-NSAMPLES:n1);
            ch2     =ch2(n1-NSAMPLES:n1);
            ch12_nok=ch12_nok(n1-NSAMPLES:n1);
        end
        
        t   = (1:length(ch1)) * T;
        subplot('Position',PosSubplotUpper);
        plot(t, ch1*ch1Gain+ch1Ofs, 'b', t, ch2*ch2Gain+ch2Ofs, 'r');
        if strcmp('dummy',titletext)
           title('Channel 1 -> blue, Channel 2 -> red'); 
        else
           title(titletext);
        end
        ylabel(ylabeltext); 
        xlabel('Time in seconds')
        grid minor
        subplot('Position',PosSubplotLower);
        stem(t, ch12_nok, 'Marker', 'none');
        set(gca,'xtick',[]); set(gca,'xticklabel',[]); set(gca,'ytick',[]); set(gca,'yticklabel',[]);
        lim = axis; lim(3) = 0; lim(4) = 1.2; axis(lim);
        text(-.01,.5,'Valid?','HorizontalAlignment','right','Units','normalized')
        assignin('base','t',       t);
        assignin('base','channel1',ch1);
        assignin('base','channel2',ch2);
        assignin('base','channel3',ch12_nok);
        
        pause(10);                  % Wait for 10 sec (approx. 13000 samples)
    end
        
    %%% End Loop while running %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%catch
%    fprintf(1,'ERROR 4: Main loop exception\n');
%    if isempty(hFig)==0, delete(hFig); end;
%    ME.message
%    ME.stack
%end
fprintf(1,'SoftScope stopped\n');
try
    if runFlag,
        if isempty(hFig)==0
            subplot('Position',PosSubplotUpper);
            title('Channel 1 -> blue, Channel 2 -> red');
            set(hStart,'Visible','off');
            set(hStop, 'Visible','on');
        end
    else
        if isempty(hFig)==0
            %subplot('Position',PosSubplotUpper);
            %title('Channel 1 -> blue, Channel 2 -> red');
            set(hStart,'Visible','on');
            set(hStop, 'Visible','off');
        end
    end
catch ME
    fprintf(1,'ERROR 5: Cleanup\n');
    ME.message
    ME.stack
end

return


function closereq(src,callbackdata)
global hFig hStart hStop hGain1 hOfs1 hGain2 hOfs2 uicTitle uicYLabel runFlag comIF ch1Gain ch2Gain ch1Ofs ch2Ofs 
runFlag = 0;
try
    SoftScopeDriver(comIF, 0);  % stop serial driver
    pause(1)
catch
    fprintf(1,'ERROR 6: SoftScopeDriver failed\n');
    ME.message
    ME.stack
end
try
    delete(hFig)
    hStart    = [];
    hStop     = [];
    hFig      = [];
    hGain1    = [];
    hGain2    = [];
    hOfs1     = [];
    hOfs2     = [];
    uicTitle  = [];
    uicYLabel = [];
catch
    fprintf(1,'ERROR 7: Cleanup failed\n');
    ME.message
    ME.stack
end
fprintf(1,'\n *** SoftScope window closed ***\n');
try
    clear SoftScopeDriver
catch
    fprintf(1,'ERROR 8: Clear SoftScopeDriver failed\n');
    ME.message
    ME.stack
end
return

function stopButtonCallback(src,callbackdata)
global hFig hStart hStop hGain1 hOfs1 hGain2 hOfs2 uicTitle uicYLabel runFlag comIF ch1Gain ch2Gain ch1Ofs ch2Ofs 
runFlag = 0;
fprintf(1,'*** SoftScope stop button callback ***\n');
set(hStart,'Visible','on');
set(hStop, 'Visible','off');
try
    SoftScopeDriver(comIF, 0);  % stop serial driver
    pause(1)
catch
    fprintf(1,'ERROR 6: SoftScopeDriver failed\n');
    ME.message
    ME.stack
end
return

function startButtonCallback(src,callbackdata)
global hFig hStart hStop hGain1 hOfs1 hGain2 hOfs2 uicTitle uicYLabel runFlag comIF ch1Gain ch2Gain ch1Ofs ch2Ofs 
fprintf(1,'*** SoftScope start button callback ***\n');
set(hStart,'Visible','off');
set(hStop, 'Visible','on');
SoftScope(comIF+1, ch1Gain, ch2Gain, ch1Ofs, ch2Ofs); % comIF == 0 cannot require us to print the list of available COM ports
return
