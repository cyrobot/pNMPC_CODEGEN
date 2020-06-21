function generateMEXCUDA
% Check for Code generation folder presence 
if(~exist('CCODEGEN','dir'))
    mkdir CCODEGEN 
end
if(~exist('CCODEGEN/include','dir'))
    mkdir CCODEGEN/include 
end
if(~exist('CCODEGEN/src','dir'))
    mkdir CCODEGEN/src 
end
if(~exist('CCODEGEN/interface','dir'))
    mkdir CCODEGEN/interface 
end
% Add current directory to path
addpath(genpath(pwd)); addpath(genpath('CCODEGEN')); 
% Flush all the old code-generation files
delete('CCODEGEN\include\*'); 
delete('CCODEGEN\src\*');
%% Building and mexing from C++ files 
srcFile = 'Source.cpp'; 
OBJ = []; fileList = dir([pwd '\pNMPC_bin\*.obj']);
for i=1:size(fileList,1)
    OBJ = [OBJ ' ' pwd '\pNMPC_bin\' fileList(i).name]; 
end
OBJ = [srcFile ' ' OBJ ];
INCLUDE = ['-I' fullfile(pwd,'\pNMPC\include')];
eval(['mex ' INCLUDE ' ' OBJ])
% Run mex generated Source mex file to generate C code 
Source
disp('============= CUDA code generation completed successfully ! ============= ');
%% MEX - CUDA-CODE
% Get all CUDA-codes 
CsrcFile = [pwd '\CCODEGEN\src\pNMPC_MEXCUDA.cu'];
% Source path 1 
SRC_PATH_1 = []; CfileList_1 = dir([pwd '\CCODEGEN\src\*.cu']);
for i=1:size(CfileList_1,1)
    if(~strcmp( CfileList_1(i).name,'pNMPC_MEXCUDA.cu' ) && ~strcmp( CfileList_1(i).name,'SQPInterface.cu' ))
        SRC_PATH_1 = [SRC_PATH_1 ' ' pwd '\CCODEGEN\src\' CfileList_1(i).name]; 
    end
end
% Source path 2 
SRC_PATH_2 = []; CfileList_2 = dir([pwd '\pNMPC\Solver\*.cu']);
for i=1:size(CfileList_2,1)
    SRC_PATH_2 = [SRC_PATH_2 ' ' pwd '\pNMPC\Solver\' CfileList_2(i).name]; 
end
% Source files
SRC = [CsrcFile ' ' SRC_PATH_1 ' ' SRC_PATH_2];
% Include files
INCLUDE_PATH_1 = ['-I' fullfile(pwd,'\CCODEGEN\include')];
INCLUDE_PATH_2 = ['-I' fullfile(pwd,'\pNMPC\Integrators')];
INCLUDE_PATH_3 = ['-I' fullfile(pwd,'\pNMPC\Parameterization')];
INCLUDE_PATH_4 = ['-I' fullfile(pwd,'\pNMPC\Solver')];

eval(['mexcuda -dynamic ' INCLUDE_PATH_1 ' ' INCLUDE_PATH_2 ' ' INCLUDE_PATH_3 ' ' INCLUDE_PATH_4  ' ' SRC ' ' 'NVCC_FLAGS=--use_fast_math']);
end