function [index min_edist RMSD] = test_face(path, db, false_possible)
    
    %load database and image
    load(db);
    img = imread(path);
    
    %reshape to vector
    z = double(reshape(img,size(img,1)*size(img,2),1));
    
    %mean deducted image
    zd = z - x_mean;
    
    %eigen coiff of test image
    ap = vk'*zd;
    
    %find the shortest eculidien dist
    edist = [];
    for i=1:size(a,2)
        ai = a(:,i);
        tmp = (norm(ai-ap))^2;
        edist = [edist tmp];
    end
    
    %find the closest image
    [min_edist index] = min(edist);
    
    if nargin < 3
       false_possible = 0; 
    end
    
    RMSD = 0;
    if false_possible == 1
        k = size(vk,2);
        recon = zeros( size(vk,1),1 );
        for l=1:k
            recon = recon + vk(:,l)*ap(l);
        end
        %reconxtruct the image
        tmp = mat2gray(reshape(recon,112,92));
        
        DifferenceImage=(mat2gray(tmp)-mat2gray(img)).^2;
        SumDiff=sum(sum(DifferenceImage));
        SumDiff=SumDiff/(112*92);
        RMSD=sqrt(SumDiff);
        
    end
    
    
end