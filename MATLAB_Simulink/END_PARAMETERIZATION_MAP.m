%
%    This file is part of pNMPC software.
%    Copyright (c) 2020 GIPSA lab [https://github.com/Kartz4code/pNMPC_CODEGEN]
%
%    Main developer - Karthik Murali Madhavan Rathai
%
%    pNMPC - A Code Generation Software Tool For Implementation of Derivative Free
%    Parameterized NMPC Scheme for Embedded Control Systems
%    The software was developed by Karthik Murali Madhavan Rathai under the supervision of
%    Mazen Alamir and Olivier Sename affiliated to Univ. Grenoble Alpes, CNRS, Grenoble INP,
%    GIPSA lab, 38000 Grenoble, France.
%
%    pNMPC software is free software: you can redistribute it and/or modify
%    it under the terms of the GNU General Public License as published by
%    the Free Software Foundation, either version 3 of the License, or
%    (at your option) any later version.
%
%    pNMPC software is distributed in the hope that it will be useful,
%    but WITHOUT ANY WARRANTY; without even the implied warranty of
%    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%    GNU General Public License for more details.
%
%    You should have received a copy of the GNU General Public License
%    along with pNMPC software.  If not, see <http://www.gnu.org/licenses/>.
%
%

function END_PARAMETERIZATION_MAP
global isParameterization;
ParameterizationMap = evalin('base','ParameterizationMap');
Fields = fieldnames(ParameterizationMap);
for i=1:size( struct2table(ParameterizationMap), 2)
    eval(['ParameterizationMap.' Fields{i} ' = ' 'vpa(eval([''ParameterizationMap(1).'' Fields{i}]),5);' ]);
    ParameterizationMapEqLHS = Fields{i};
    assignin('base', ParameterizationMapEqLHS, vpa(eval(['ParameterizationMap(1).' Fields{i}]),5));
end
if(isParameterization)
    save('pNMPC_bin/ParameterizationMap', 'ParameterizationMap');
end
isParameterization = 0;
end
