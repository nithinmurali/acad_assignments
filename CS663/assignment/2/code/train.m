function [a,v,x_mean] = train(databse_top_path, database, k, offset)
    h = 0;
    w = 0;
    i = 0;
    
    img_db_path = strcat(databse_top_path,'/',database);

    % choose the images form the respective database
    if strcmp(database,'att_faces')
        
        for subject=1:35
            for images=1:5
                i=i+1;
                cur_img = fullfile(img_db_path,strcat('s',num2str(subject)),strcat(num2str(images),'.pgm'));
                image = imread(cur_img);
                if subject ==1 && images == 1
                    w = size(image,2);
                    h = size(image,1);
                end
              imgVec(1:w*h,i)  = reshape(image,w*h,1);
            end
        end
    end
    
    if strcmp(database,'CroppedYale')
        snum=1:39;
        total_files = 0;
        for subject = snum(snum~=14)
            D = dir(strcat(img_db_path,'/',strcat('yaleB',num2str(subject,'%02i'))));
            i=0;
            file = 1;
            while file<=30
                i = i+1;
                if not(strcmp(D(i).name,'.')|strcmp(D(i).name,'..')|strcmp(D(i).name,'Thumbs.db'))
                    file = file + 1;
                    total_files = total_files + 1;
                    cur_img = fullfile(img_db_path,strcat('yaleB',num2str(subject,'%02i')),D(i).name);
                    image = imread(cur_img);
                    if w == 0
                        w = size(image,2);
                        h = size(image,1);
                    end
                    imgVec(1:w*h,total_files)  = reshape(image,w*h,1);    
                end              
            end
        end
            
    end
    
    % now we have the vectors get the mean

    x_mean = mean(imgVec,2);
    num_images = size(imgVec,2);
    
    for i=1:num_images
        t = double(imgVec(:,i));
        x_dev(:,i) = t - x_mean;
    end
    
    % get the eigen vectors of L
    L = x_dev'*x_dev;
    [vv dd]=eig(L); %vv is the eigen vector while dd is the eigen value
    
    %sort the eigen vectors according to the eigen values
    for l =1:size(dd,2)-1
       for j=1:(size(dd,2)-l)
           if dd(j,j)<dd(j+1,j+1)
                tmp = dd(j,j);
                dd(j,j) = dd(j+1,j+1);
                dd(j+1,j+1) = tmp;
                
                tmpv = vv(:,j);
                vv(:,j) = vv(:,j+1);
                vv(:,j+1)=tmpv;
           end
       end
    end
    
    
    %obtain the eigen vect of C ie. the eigen faces
    v = x_dev*vv;
    
    %normalize
    for i=1:size(v,2)
        elem = v(:,i);
        ssq = sum(sqrt(elem.^2));
        v(:,i) = v(:,i)./ssq;
    end
    
    %choose required eigen vectors
    vk = v(:,(1+offset):(k+offset));
    
    %finding the eigen coiff for the training set
    a = [];
    for i=1:num_images
        ai = vk'*double(x_dev(:,i));
        a=[a ai];
    end
    
    %save the train data
    save(strcat(database,'_k=',num2str(k),'_train_data.mat'),'a','vk','x_mean');
    
end