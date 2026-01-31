% Digital Image Correlation - Visualization of Data
%
% Author: David Gayman
% Date: 12-31-2011
%
% Instructions
%   1. Run ./correlate with option flags, as described in the
%        documentation, CORRELATE_Manual.pdf
%   2. In the internally-specified output directory, the
%        'positionField.txt' and 'strainField.txt' files will be updated
%   3. Run this script from the output directory
%   4. Visual output will be generated
%
% Interpretation of output
% %   3D Plot:
% %     -Each dot has been identified by the c++ code as a "dot"
% %     -A line connects each dot with its corresponding dot from the
% %     previous image
% %     -If a dot's location has been stored as (-1, -1), it is considered
% %     invalid and is not plotted; thus, points which have no line
% %     connecting to them signify one or more unidentified dots in previous
% %     images
% %     -Black dots are in the first image, called the reference image
% %     -Green dots have not migrated more than migration_dist_thresh from
% %     the previous image (or identified location)
% %     -Red dots have migrated excessively


%% Initialize
clc
in1 = importdata('../data/positionField.txt');
maxNumBlobs = in1(1,1);
gridMap = in1(2:maxNumBlobs+1, 1:end-1);
positionField = in1(maxNumBlobs+2:end, 1:2);
strainField = importdata('../data/strainField.txt');


%% Iterate over time steps and display strain surfaces
for t = 1:12
    
    % Initialize a strain field matrix for display
    sFDisp = zeros(maxNumBlobs,4);
    
    % Loop over points in position field
    for p = 1:maxNumBlobs
        
        if positionField((t-1)*maxNumBlobs + p, 1) ~= -1
            
            % If position is valid, then store the strain value
            sFDisp(p,:) = strainField((t-1)*maxNumBlobs + p, :);
            
        else
            
            % Else, store NaN
            sFDisp(p,:) = NaN;
            
        end
        
    end
    
    % Display strain field
    figure(1); clf; hold on;
    for p = 1:maxNumBlobs
        
        if sum(isnan( sFDisp(p,:) )) <= 0
            
            X = positionField((t-1)*maxNumBlobs + p, 1);
            Y = positionField((t-1)*maxNumBlobs + p, 2);
            Z1 = sFDisp(p,1);
            Z2 = sFDisp(p,2);
            Z3 = sFDisp(p,3);
            Z4 = sFDisp(p,4);
            
            figure(1); subplot(2,2,1); hold on; plot3(X, Y, Z1, '*')
            view(3);
%             A = axis;
%             axis([A(1) A(2) A(3) A(4) min(A(5), Z1) max(A(6), Z1) ])
%             axis([A(1) A(2) A(3) A(4) -0.01 0.01])
            figure(1); subplot(2,2,2); hold on; plot3(X, Y, Z2, '*')
            view(3);
            figure(1); subplot(2,2,3); hold on; plot3(X, Y, Z3, '*')
            view(3);
            figure(1); subplot(2,2,4); hold on; plot3(X, Y, Z4, '*')
            view(3);
            
        end
        
    end
    
    % Pause
    pause
    
end


