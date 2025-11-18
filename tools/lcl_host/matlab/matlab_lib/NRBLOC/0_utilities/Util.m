%------------------------------------------------------------------------------
% Copyright : Stichting imec Nederland (http://www.imec.nl)
%             *** IMEC CONFIDENTIAL ***
%------------------------------------------------------------------------------
classdef (HandleCompatible) Util < matlab.mixin.Copyable
    % Description
    %   This Base-class' is used to provide baisc funtionality to
    %   other classes and prevent double code. Classes derived/inheriting
    %   from this class will be automatically be handle-based, and have 
    %   'general-purpose' methods like copy, disp, set and get. 
    %   Programmers using this class for inheritance may always override the inherent methods
    %   by redefining these methods in the derived class. For instance to
    %   speed up functionality.
    %   See also DiscreteTime
    % Description end
    properties
    end
    properties (Hidden)
       DATA4plot
       STOREDATA=false;
    end
    methods
        function PropertyValue = get(obj,PropertyName)
            if isprop(obj,PropertyName)
                if ismethod(obj,['get_' PropertyName])
                    STR2EVAL=['PropertyValue=get_' PropertyName '(obj);'];
                    eval(STR2EVAL);
                else
                    PropertyValue=obj.(PropertyName);
                end                
            else
                error(['Class ' class(obj) ' has no property with the name' PropertyName])
            end
        end
        function [] = set(obj,PropertyName,PropertyValue)
            if isprop(obj,PropertyName)
                if ismethod(obj,['set_' PropertyName])
                    STR2EVAL=['set_' PropertyName '(obj,PropertyValue);'];
                    eval(STR2EVAL);
                else
                    obj.(PropertyName) = PropertyValue;
                end
            else
                error(['Class ' class(obj) ' has no property with the name' PropertyName])
            end
        end

        function varargout = disp(self,DisplayLevel)
            if nargin==1
                DisplayLevel = 0;
                InitText = sprintf('%s = \t [class %s]\n', inputname(1), class(self));
            end
            TEXT = Util.parse(self, DisplayLevel);
            if nargout==0
                fprintf([InitText, TEXT, '\n']);
            elseif nargout==1
                varargout(1) = {[InitText, TEXT]};
            else
                error('only 1 output argument possible')
            end
        end % disp

        function ResetDebugData(obj)
            obj.DATA4plot       = [];
            meta = eval(['?',class(obj)]);
            for p = 1: size(meta.Properties,1)
                pname = meta.Properties{p}.Name;
                str2eval=['TypeObject=isa(obj.' pname ',''Util'');'];
                eval(str2eval);
                if TypeObject
                    str2eval=['ResetDebugData(obj.' ,pname, ');'];
                    eval(str2eval);
                end
            end
            
        end
        
%         function copyout = copy(obj)
%             % Create a shallow copy of the calling object.
%             copyout = eval(class(obj));
%             meta = eval(['?',class(obj)]);
%             for p = 1: size(meta.Properties,1)
%                 pname = meta.Properties{p}.Name;
%                 if isobject(obj.(pname))
%                     if ~meta.Properties{p}.NonCopyable
%                         copyout.(pname) = copy(obj.(pname));
%                     end
%                 else
%                     if ~meta.Properties{p}.NonCopyable
%                         copyout.(pname) = obj.(pname);
%                     end
%                 end
%             end
%         end  %copy
        
        function reset(self)
            S = properties(self);
            for k=1:length(S)
                if ismethod(self.(S{k}), 'reset')
                    self.(S{k}).reset();
                end
            end
        end
    end
    
    methods (Static)
        function TEXT = parse(val, DisplayLevel)
            if nargin==1
                DisplayLevel = 0;
            end
            Indent    = char(32*ones(1,3));
            Offset    = repmat(Indent, 1, DisplayLevel);
            AllFields = fields(val);
            FieldNames = char(AllFields);           % In this one, all fieldnames ave the same length
            
            TEXT      = '';
            for cnt=1:length(AllFields)
                temp = val.(AllFields{cnt});
                if isobject(temp) || isstruct(temp)
                    NewText = Util.parse(temp, DisplayLevel+1);
                    TEXT    = sprintf('%s%s%s%s   [class %s]\n%s', TEXT, Indent, Offset, FieldNames(cnt, :), class(temp), NewText);
                elseif ischar(temp)
                    TEXT = sprintf('%s%s%s%s = ''%s''', TEXT, Indent, Offset, FieldNames(cnt, :), temp);
                elseif isscalar(temp) && isnumeric(temp)
                    TEXT = sprintf('%s%s%s%s = %s', TEXT, Indent, Offset, FieldNames(cnt, :), num2str(temp));
                elseif isempty(temp)
                    TEXT = sprintf('%s%s%s%s = []', TEXT, Indent, Offset, FieldNames(cnt, :));
                elseif isnumeric(temp)
                    NewTxT = '[';
                    for k=1:ndims(temp)
                        NewTxT = [NewTxT, num2str(size(temp, k)), 'x'];     %#ok<AGROW>
                    end
                    NewTxT = [NewTxT(1:end-1), ' ', class(temp), ']'];
                    TEXT = sprintf('%s%s%s%s = %s', TEXT, Indent, Offset, FieldNames(cnt, :), NewTxT);
                elseif islogical(temp)
                    LogicalStr = {'false', 'true'};
                    TEXT = sprintf('%s%s%s%s = %s', TEXT, Indent, Offset, FieldNames(cnt, :), LogicalStr{temp + 1});
                else
                    TEXT = sprintf('%s%s%s%s', TEXT, Indent, Offset, FieldNames(cnt, :));
                end
                if cnt < length(AllFields)
                    TEXT = [TEXT '\n' ];                                    %#ok<AGROW>
                end
            end
        end
    end
end