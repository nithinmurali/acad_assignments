
db = './att_faces_k=175_train_data.mat';
load(db);

%% face reconstruction
index = 1;

%TODO chnage to uigetdir
 img = imread('../images/databases/att_faces/s1/1.pgm');

%reshape to vector
z = double(reshape(img,size(img,1)*size(img,2),1));

%mean deducted image
zd = z - x_mean;

%eigen coiff of test image
ap = vk'*zd;

i=0;
hFig =figure('Name','reconstruction');set(gcf, 'Position', get(0,'Screensize'));
for k = [2, 10, 20, 50, 75, 100, 125, 150, 175]
    i=i+1;
    eig_faces = vk(:,1:k);
    recon = zeros( size(eig_faces,1),1 );
    for l=1:k
        recon = recon + eig_faces(:,l)*ap(l);
    end
    tmp = mat2gray(reshape(recon,112,92));
    subplot(3,3,i)
    imshow(tmp)
    drawnow;
    
    nFig = figure('Name','Reconstruction');set(gcf, 'Position', get(0,'Screensize'));
    imshow(tmp)
    title(strcat('k=',num2str(i),' Subject1 1'))
    
    [cdata, colorMap] = getframe(nFig);
    imwrite(cdata,strcat('../images/Reconstruction/','k=',num2str(k),'_Subject_1_1','.png'),'png');
    close(nFig);
end

    [cdata, colorMap] = getframe(hFig);
    imwrite(cdata,strcat('../images/Reconstruction/','Subject_1_1_Subplot','.png'),'png');
%% plot top 25 eigen faces

hFig =figure('Name','Eigen faces','NumberTitle','off');set(gcf, 'Position', get(0,'Screensize'));
for i =1:25
    tmp = mat2gray(reshape(vk(:,i),112,92));
    subplot(5,5,i)
    imshow(tmp)
    drawnow;
    
    nFig = figure('Name','EigenFaces');set(gcf, 'Position', get(0,'Screensize'));
    imshow(tmp)
    title(strcat('k=',num2str(i),' Eigen Face'))
    
    [cdata, colorMap] = getframe(nFig);
    imwrite(cdata,strcat('../images/EigenFaces/','k=',num2str(i),'EigenFace','.png'),'png');
    close(nFig);
end

    [cdata, colorMap] = getframe(hFig);
    imwrite(cdata,strcat('../images/EigenFaces/','Eigen_Subplot','.png'),'png');

%% fourier transform plot

hFig = figure('Name','fourier plot','NumberTitle','off');set(gcf, 'Position', get(0,'Screensize'));
for i =1:25
    tmp = reshape(vk(:,i),112,92);
    tmp_fft = fft2(tmp);
    fft_img = abs(fftshift(tmp_fft));
    fft_log = mat2gray(log(fft_img + 1));
    subplot(5,5,i)
    imshow(fft_log)
    drawnow;
    
    nFig = figure('Name','Reconstruction');set(gcf, 'Position', get(0,'Screensize'));
    imshow(fft_log)
    title(strcat('k=',num2str(i),' Eigen FFT Image'))
    [cdata, colorMap] = getframe(nFig);
    
    imwrite(cdata,strcat('../images/EigenFFT/','k=',num2str(i),'Eigenfft','.png'),'png');
    close(nFig);
end

    [cdata, colorMap] = getframe(hFig);
    imwrite(cdata,strcat('../images/EigenFFT/','Eigenfft_Subplot','.png'),'png');
    