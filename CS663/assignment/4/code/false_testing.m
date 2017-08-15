%% false positive/negtive testing

database_top_folder = uigetdir('../images');
database = 'att_faces';
false_positive=0;
false_negative=0;
k = 30;
threshold = 650;

recog_rate = 0;

for subject=1:40
    for images=1:10
        if ~(subject < 36 && images < 6)
           
            cur_img = fullfile(database_top_folder,database,strcat('s',num2str(subject)),strcat(num2str(images),'.pgm'));
            [index min_dist RMSD] = test_face(cur_img,strcat(database,'_k=',num2str(k),'_train_data.mat'),1);
            if min_dist > threshold && ismember(index,(subject-1)*5+1:(subject*5))
                false_negative = false_negative+1;

            end
            if min_dist < threshold && ~ismember(index,(subject-1)*5+1:(subject*5))
                false_positive = false_positive + 1;
            end
        end
    end
end

