% Laborversuch: Motorboard
% Regelungstechnik 2
% (C) 2019 W.Lindermeir, W.Zimmermann
% Hochschule Esslingen
%
% Level-2 MATLAB file S-Function (nach Beispiel msfuntmpl_basic.m)
% zur Darstellung einer Linearachse
% (Kann nur einmal in einem Simulink-Blockschaltbild verwendet werden)
%

function Linearachse_L2(block)
  setup(block);
%endfunction

function setup(block)
  %% Register number of input and output ports
  block.NumInputPorts  = 1;
  block.NumOutputPorts = 1;
  %% Setup functional port properties to dynamically inherited.
  block.SetPreCompInpPortInfoToDynamic;
  block.SetPreCompOutPortInfoToDynamic;
  %% Override input port properties
  block.InputPort(1).Dimensions  = 1;
  block.InputPort(1).DatatypeID  = 0;  % double
  block.InputPort(1).Complexity  = 'Real';
  block.InputPort(1).DirectFeedthrough = true;
  % Override output port properties
  block.OutputPort(1).Dimensions  = 1;
  block.OutputPort(1).DatatypeID  = 0; % double
  block.OutputPort(1).Complexity  = 'Real';
  %% Register parameters
  block.NumDialogPrms  = 4;
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
  block.NumDworks = 3;
  block.Dwork(1).Name = 'enable';            block.Dwork(1).DatatypeID = 8; % bool
  block.Dwork(2).Name = 'spindelsteigung';   block.Dwork(2).DatatypeID = 0; % double
  block.Dwork(3).Name = 'breite_halb';       block.Dwork(3).DatatypeID = 0; % double
  for i = 1:3
     block.Dwork(i).Dimensions      = 1;
     block.Dwork(i).Complexity      = 'Real';
     block.Dwork(i).UsedAsDiscState = false;
  end
%endfunction

function Start(block)
  global linearachsenfigure h__ phi_sollG phiMaxG
  %% Initialize Dwork
  block.Dwork(1).Data = logical(block.DialogPrm(1).Data);  % enable
  phiMaxG             = block.DialogPrm(3).Data;
  block.Dwork(2).Data = block.DialogPrm(4).Data;  % spindelsteigung
  block.Dwork(3).Data = 1.5;  % breite_halb
  enable          = block.Dwork(1).Data;
  spindelsteigung = block.Dwork(2).Data;
  breite_halb     = block.Dwork(3).Data;
  phi_sollG = 0;
  linearachsenfigure = findobj('Type','figure','Name','Linearachse');
  if ~isempty(linearachsenfigure)
     close(linearachsenfigure);
  end
  if enable == 1,
     linearachsenfigure=figure;
     set(linearachsenfigure,'Name','Linearachse');
     clf
     axis equal;
     axis([-phiMaxG/2/pi*spindelsteigung-breite_halb phiMaxG/2/pi*spindelsteigung+breite_halb 0 phiMaxG/2/pi*spindelsteigung*0.2]);
     % Erzeugt den Werkstuecktraeger
     h__ = rectangle('Position',[-breite_halb,0,2*breite_halb,phiMaxG/2/pi*spindelsteigung*0.2], 'Curvature',[0.1,0.1], 'FaceColor','r');
     title('Linearachse');
     xlabel('Position x [mm]');
     len=0.1; y_but = 0.1;
     uicontrol('Style','pushbutton','Units','normalized','Position',[0.0+0.05 y_but len len],  'String','MIN',   'Interruptible','off','Callback',{@Button_update_fkt,1} );
     uicontrol('Style','pushbutton','Units','normalized','Position',[0.25 y_but len len],      'String','-Mitte','Interruptible','off','Callback',{@Button_update_fkt,2} );
     uicontrol('Style','pushbutton','Units','normalized','Position',[0.5-len/2 y_but len len], 'String','O',     'Interruptible','off','Callback',{@Button_update_fkt,3} );
     uicontrol('Style','pushbutton','Units','normalized','Position',[0.75-len y_but len len],  'String','+Mitte','Interruptible','off','Callback',{@Button_update_fkt,4} );
     uicontrol('Style','pushbutton','Units','normalized','Position',[1-len-0.05 y_but len len],'String','MAX',   'Interruptible','off','Callback',{@Button_update_fkt,5} );
     hold on
  end;
%endfunction

function Update(block)
  global linearachsenfigure h__ phiMaxG
  persistent uold 
  u = block.InputPort(1).Data;
  enable          = block.Dwork(1).Data;
  spindelsteigung = block.Dwork(2).Data;
  breite_halb     = block.Dwork(3).Data;
  if enable == 1 && any(get(0,'Children')==linearachsenfigure),
     if strcmp(get(linearachsenfigure,'Name'),'Linearachse'),
        set(0,'currentfigure',linearachsenfigure);
        if u~=uold
            set(h__, 'Position',[-breite_halb+u/2/pi*spindelsteigung, 0, 2*breite_halb, phiMaxG/2/pi*spindelsteigung*0.2]);
        end
        uold=u;
     end
  end
  
%endfunction

function Output(block)
  global phi_sollG
  block.OutputPort(1).Data = phi_sollG;
%endfunction

function Terminate(block)
  global linearachsenfigure 
  enable = block.Dwork(1).Data;
  if enable == 1 && any(get(0,'Children')==linearachsenfigure),
     if strcmp(get(linearachsenfigure,'Name'),'Linearachse'),
        set(0,'currentfigure',linearachsenfigure);
        hold off
     end
  end
%end Terminate


function Button_update_fkt(src,event,but_pressed)
global phi_sollG phiMaxG
if     but_pressed == 1
    phi_sollG= phiMaxG / 2 / pi * 400;
elseif but_pressed == 2
    phi_sollG= phiMaxG/2 / 2 / pi * 400;
elseif but_pressed == 3
    phi_sollG= 0 / 2 / pi * 400;
elseif but_pressed == 4
    phi_sollG= -phiMaxG/2 / 2 / pi * 400;
elseif but_pressed == 5
    phi_sollG= -phiMaxG / 2 / pi * 400;
end
