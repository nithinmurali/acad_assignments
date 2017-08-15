clear all
clc
close all

recog_rate1 = [];
recog_rate2 = [];

database_top_folder = uigetdir('../images');

% %% find recognition rates for att_faces database
% database = 'att_faces';
% count = 0;
% for k = [1, 2, 3, 5, 10, 20, 30, 50, 75, 100, 125, 150, 170]
% %     train(database_top_folder,database,k,0);
%     count = count +1;
%     recog_rate = 0;
%     for subject=1:35
%         for images=6:10
%             cur_img = fullfile(database_top_folder,database,strcat('s',num2str(subject)),strcat(num2str(images),'.pgm'));
%             [index min_dist] = test_face(cur_img,strcat(database,'_k=',num2str(k),'_train_data.mat'));
%             recog_rate = recog_rate + ismember(index,(subject-1)*5:(subject*5));
%         end
%     end
%     recog_rate1(count) = (recog_rate/(35*5))*100;
% end

%% find recognition rates for croppedYale database
database = 'CroppedYale';
snum=1:39;
count = 0;

for k = [1, 2, 3, 5, 10, 20, 30, 50, 60, 65, 75, 100, 200, 300, 500, 1000]
    snum=1:39;
    total_files = 0;
    recog_rate=0;
    for subject = snum(snum~=14)
        D = dir(strcat(database_top_folder,'/',database,'/',strcat('yaleB',num2str(subject,'%02i'))));
        i=0;
        file = 1;
        while file<=30
            i = i+1;
            if not(strcmp(D(i).name,'.')|strcmp(D(i).name,'..')|strcmp(D(i).name,'Thumbs.db'))
                file = file + 1;
                total_files = total_files + 1;
                cur_img = fullfile(database_top_folder,database,strcat('yaleB',num2str(subject,'%02i')),D(i).name);
                [index min_dist] = test_face(cur_img,strcat(database,'_k=',num2str(k),'_train_data.mat'));
                recog_rate = recog_rate + ismember(index,(subject-1)*5:(subject*5));
                
            end              
        end
    end

    
    recog_rate2(count) = (recog_rate/(38*30))*100;
end

%remove the databases generated
%delete('./*.mat')

%% plot both the rates
plot(recog_rate1);
plot(recog_rate2);


