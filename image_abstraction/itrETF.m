function [new_etf] = itrETF(inputImage, iter)
    
    %find gradient g(x)
    gmap = zeros(size(inputImage,1), size(inputImage,2), 2); 
    [gmap(:,:,1), gmap(:,:,2)] = imgradientxy(inputImage);

    %find t(x)
    t = zeros(size(gmap));
    for i=1:size(gmapx,1)
        for j=1:size(gmapx,2)
           norm = sqrt( gmap(i,j,0)*gmap(i,j,0) + gmap(i,j,1)*gmap(i,j,1) );
           t(i,j,1) = (-1)*gmap(i,j,0)/norm;
           t(i,j,0) = gmap(i,j,1)/norm;
           
        end
    end
    
   new_etf = ETF(inputImage,gmap,t,10);
   
   for k=2:iter
       new_etf = EFT(new_etf,gmap,t,10);
   end
   
end

function EFTimage = ETF(InputImage,gmap,tmap,Kernel)

    Length = size(InputImage,1);
    width = size(InputImage,2);
    range=floor((Kernel-1)/2);
    EFTimage = zeros(Length,width);
    
    for i=1:Length
        %setting the Lengthwise range for window
        lower_i=max(1,i-range);
        higher_i=min(Length,i+range);
        for j=1:width

            %setting the breadthwise range for window
            lower_j=max(1,j-range);
            higher_j=min(width,j+range);

            %retreiving the sub-image
            sub_image=InputImage(lower_i:higher_i,lower_j:higher_j);
            
            %Applay EFT Construction filter
            for ki=1:size(sub_image,1)
                for kj=1:size(sub_image,2)
                    
                    if sum([i j].*[ki ky]>0)
                        sign = 1;
                    else
                        sign = -1;
                    end
                    ws = weightSpatial(i,j,ik,iy);
                    %wm = 0.5*( gmap(i,j)- gmap(ki,kj) + 1);
                    wm = weightMag(gmap(i,j,1),gmap(i,j,2), gmap(ki,kj,1),gmap(ki,kj,2));
                    wd = fabs(sum( tmap(i,j,:).*tmap(ki,kj,:)) );
                    newVal = double( sign*ws*wd*wm*tmap(i,j) );
                    
                end
            end
            EFTimage(i,j) = newVal;
        end
    end

end

function wm= weightMag(mx1,mx2,my1,my2)
    nx = sqrt(mx1*mx1+mx2*mx2);
    ny = sqrt(my1*my1+my2*my2);
    wm = 0.5*(ny-nx+1);
end

function test = weightSpatial(x1,y1,x2,y2, mu)

    if(sqrt( (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2) ) < mu)
        test = 1;
    else
        test = 0;
    end
end
