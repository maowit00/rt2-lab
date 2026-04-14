% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Level-2 MATLAB file S-Function (nach Beispiel msfuntmpl_basic.m)
% zur Darstellung einer Tachoanzeige
% (Kann nur einmal in einem Simulink-Blockschaltbild verwendet werden)
%
function Tacho_Welle_L2(block)
  setup(block);
%endfunction

function setup(block)
  %% Register number of input and output ports
  block.NumInputPorts  = 1;
  block.NumOutputPorts = 0;
  %% Setup functional port properties to dynamically inherited.
  block.SetPreCompInpPortInfoToDynamic;
  block.SetPreCompOutPortInfoToDynamic;
  %% Override input port properties
  block.InputPort(1).Dimensions  = 1;
  block.InputPort(1).DatatypeID  = 0;  % double
  block.InputPort(1).Complexity  = 'Real';
  block.InputPort(1).DirectFeedthrough = true;
  %% Register parameters
  block.NumDialogPrms  = 6;
  % Register sample times
  block.SampleTimes = [block.DialogPrm(2).Data 0];
  %% Set the block simStateCompliance to default (i.e., same as a built-in block)
  block.SimStateCompliance = 'DefaultSimState';
  %% Register methods
  block.RegBlockMethod('PostPropagationSetup', @DoPostPropSetup);
  block.RegBlockMethod('Start',                @Start);
  block.RegBlockMethod('Update',               @Update);  
  block.RegBlockMethod('Outputs',              @Output);    % Required
  block.RegBlockMethod('Terminate',            @Terminate); % Required
%endfunction

function DoPostPropSetup(block)
  %% Setup Dwork
  block.NumDworks = 6;
  block.Dwork(1).Name = 'enable';            block.Dwork(1).DatatypeID = 8; % bool
  block.Dwork(2).Name = 'Abtastzeit';        block.Dwork(2).DatatypeID = 0; % double
  block.Dwork(3).Name = 'Minimalwert';       block.Dwork(3).DatatypeID = 0; % double
  block.Dwork(4).Name = 'Maximalwert';       block.Dwork(4).DatatypeID = 0; % double
  block.Dwork(5).Name = 'Skalenteilung';     block.Dwork(5).DatatypeID = 0; % double
  block.Dwork(6).Name = 'Skalierungsfaktor'; block.Dwork(6).DatatypeID = 0; % double
  for i = 1:6
     block.Dwork(i).Dimensions      = 1;
     block.Dwork(i).Complexity      = 'Real';
     block.Dwork(i).UsedAsDiscState = false;
  end
%endfunction

function Start(block)
global tachofigure 
  %% Initialize Dwork
  block.Dwork(1).Data = logical(block.DialogPrm(1).Data);  % enable
  block.Dwork(2).Data = block.DialogPrm(2).Data;  % Abtastzeit
  block.Dwork(3).Data = block.DialogPrm(3).Data;  % Minimalwert
  block.Dwork(4).Data = block.DialogPrm(4).Data;  % Maximalwert
  block.Dwork(5).Data = block.DialogPrm(5).Data;  % Skalenteilung
  block.Dwork(6).Data = block.DialogPrm(6).Data;  % Skalierungsfaktor
  enable  = block.Dwork(1).Data;
  minimum = block.Dwork(3).Data;
  maximum = block.Dwork(4).Data;
  teilung = block.Dwork(5).Data;
  tachofigure = findobj('Type','figure','Name','Tacho');
  if ~isempty(tachofigure)
     close(tachofigure);
  end
  if enable == 1,
     tachofigure=figure;
     set(tachofigure,'Name','Tacho');
     cla(gca,'reset');
     axis(gca,'off');
     axis([-1.2 1.2 -1.2 1.2]);
     hold on;
     i=-0.25*pi:pi/100:1.25*pi;
     x=cos(i);
     y=sin(i);
     plot(x,y); % plotten der Kreislinie
     for i=-0.25*pi:3/2*pi*teilung/(maximum-minimum):1.25*pi
        plot([-cos(i) -0.9*cos(i)],[sin(i) 0.9*sin(i)]);   % plotten der Teilungen
     end;
     plot([-cos(pi/2) -0.9*cos(pi/2)],[sin(pi/2) 0.9*sin(pi/2)], 'r', 'LineWidth',3);  % Plotten des Null-Ticks
     text(-1,-0.8,num2str(minimum));       % min
     text(-1.25,0.4,num2str(minimum/2));   % min/2 
     text(-0.05,1.1,num2str(0));           % 0%
     text(1.00,0.4,num2str(maximum/2));    % max/2 
     text(0.75,-0.8,num2str(maximum));     % max
     x=[0  0];
     y=[0  1];
     h = animatedline(x,y,'LineWidth',5);
     set(gca,'UserData',h);
  end;
%endfunction

function Update(block)
  
%endfunction

function Output(block)
global tachofigure 
  enable  = block.Dwork(1).Data;   % enable
  minimum = block.Dwork(3).Data;
  maximum = block.Dwork(4).Data;
  k       = block.Dwork(6).Data;   % Skalierungsfaktor
  u       = block.InputPort(1).Data;
  if enable == 1 && any(get(0,'Children')==tachofigure),
      if strcmp(get(tachofigure,'Name'),'Tacho'),
        set(0,'currentfigure',tachofigure);
        h = get(gca,'UserData');
        v=k*u;
        if v<minimum
           v=minimum;
        end;
        if v>maximum
           v=maximum;
        end;
        x=[0 -cos(1.25*pi+(v+minimum)/(maximum-minimum)*1.5*pi)];
        y=[0  sin(1.25*pi+(v+minimum)/(maximum-minimum)*1.5*pi)];
        clearpoints(h);
        addpoints(h,x,y)
        drawnow;
      end
    end
 
%endfunction

function Terminate(block)

%end Terminate

