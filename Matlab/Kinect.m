vidReader = VideoReader('Kinect_01.avi');
% vidWriter = VideoWriter('KinectDemo.mp4');
% open(vidWriter);
i = 0;
x_prev = [0,0,0,0,0];
y_prev = [0,0,0,0,0];

figure(1);
while hasFrame(vidReader)&&i<150
    frame = rgb2gray(readFrame(vidReader));
    i = i + 1;
    
    [hand_fr, hand_thresh] = palmTracking(frame);
    [x,y,x_cen,y_cen] = fingerTracking(hand_fr, hand_thresh); 
    x_2 = [];
    y_2 = [];
    if length(x) < 5        
        [x_2,y_2] = depthTracking(hand_fr);
    end
    
    %Boundaries
    %Index Finger 1,1,55,70
    bound = [1,size(hand_fr,2)*0.4,1,70];
    [X(1),Y(1)] = predict(bound,x,y,x_2,y_2,x_prev,y_prev,1);
    %Middle Finger 40,1,60,70
    bound = [size(hand_fr,2)*0.45,size(hand_fr,2)*0.65,1,70];
    [X(2),Y(2)] = predict(bound,x,y,x_2,y_2,x_prev,y_prev,2);
    %Ring Finger 50,1,70,70
    bound = [size(hand_fr,2)*0.6,size(hand_fr,2)*0.8,1,70];
    [X(3),Y(3)] = predict(bound,x,y,x_2,y_2,x_prev,y_prev,3);
    %Pinky Finger 60,1,max,70
    bound = [size(hand_fr,2)*0.8,size(hand_fr,2),1,70];
    [X(4),Y(4)] = predict(bound,x,y,x_2,y_2,x_prev,y_prev,4);
    %Thumb
    bound = [1,size(hand_fr,2)*0.6,20,100];
    [X(5),Y(5)] = predict(bound,x,y,x_2,y_2,x_prev,y_prev,5);
    
    for ii = 1:length(X)-1
        dist = sqrt((Y(ii) - Y(ii+1))^2 + (X(ii) - X(ii+1))^2);
        if dist < 10
            X(ii) = x_prev(ii);
            Y(ii) = y_prev(ii);
        end
    end
    
    x_prev = X;
    y_prev = Y;
    
    radii = [];
    for ii = 1:length(X)
        radii(ii) = 1;
    end
    
    subplot(1,2,1);
    imshow(hand_fr);hold on;
    viscircles([x_cen;y_cen]', 5,'EdgeColor','r');
    viscircles([X;Y]', radii,'EdgeColor','r'); hold off;

    subplot(1,2,2);
    imshow(zeros(size(hand_fr))); hold on;
    for ii = 1:length(X)
        line([X(ii), x_cen], [Y(ii), y_cen], 'Color', 'w');
    end
    hold off;

    %fr = getframe();
    %writeVideo(vidWriter, fr);
    pause(0.2);
  
    
end

function [X,Y] = predict(bnd,x,y,x_2,y_2,x_prev,y_prev,i)
    jj = 1; temp = [];
    X = 0;
    Y = 0;
    for ii = 1:length(x)
       if x(ii) > bnd(1) && x(ii) < bnd(2) && y(ii) > bnd(3) && y(ii) < bnd(4)
           temp(jj) = ii;
           jj = jj + 1;
       end
       if temp > length(x)
           temp = [];
       end
       if(length(temp)==1)
           X = x(temp);
           Y = y(temp);
       elseif(length(temp)>1&&length(temp)<length(x))
           X = x(temp(end));
           Y = y(temp(end));
       elseif(isempty(temp)&&~isempty(x_2))
           jj = 1; temp = [];
           for iii = 1:length(x_2)
               if x_2(iii) > bnd(1) && x_2(iii) < bnd(2) && y_2(iii) > bnd(3) && y_2(iii) < bnd(4)
                    temp(jj) = iii;
                    jj = jj + 1;
               end
           end
           if(length(temp)==1)
               X = x_2(temp);
               Y = y_2(temp);
           end
       else
          X = x_prev(i);
          Y = y_prev(i);
       end
    end
end

function [y,z] = palmTracking(frame)
    %This parts creates the bounding box for our hand
    N = histogram(frame);
    thresh = (max(N.BinEdges)/256)*0.5;

    im_bin = imbinarize(frame, thresh);

    se = strel('disk',2);
    im_morph = imclose(im_bin,se);

    L = logical(im_morph);
    Stats = regionprops(L,'Area','ConvexImage','FilledImage','BoundingBox');

    temp = 1:length(Stats);
    for ii = 1:length(Stats)
        temp(ii) = Stats(ii).Area;
    end
    [~,I] = max(temp);
    
    im_crop = Stats(I).FilledImage;
    %im_bound = boundarymask(im_crop);

    KIN_crop = imcrop(frame,Stats(I).BoundingBox);
        
    %Vertical Scan
    delta = 1:size(im_crop,1);
    II_left = 0;
    II_right = 0;
    for ii = 1:size(im_crop,1)
        for jj_left = 1:size(im_crop,2)
            if im_crop(ii,jj_left) ==1
                II_left = jj_left;
                break;
            end
        end
        for jj_right = size(im_crop,2):-1:1
            if im_crop(ii,jj_right) == 1
                II_right = jj_right;
                break;
            end
        end
        delta(ii) = II_right-II_left;
    end
    
    delta_movavg = smooth(delta(1:size(delta,2)-50),10);
    [PKS, LOCS] = findpeaks(delta_movavg,'MinPeakDistance', 20);
    [~,I_max] = max(PKS);
    [~,II] = min(PKS(I_max:end));
    II = II + (I_max-1);
    %Create crop of only palm
    im_crop2 = imcrop(im_crop,[1,1,size(im_crop,1),LOCS(II)]);
    
    L = logical(im_crop2);
    Stats = regionprops(L,'Area','ConvexImage','FilledImage','BoundingBox');

    temp = 1:length(Stats);
    for ii = 1:length(Stats)
        temp(ii) = Stats(ii).Area;
    end
    [~,I2] = max(temp);

    im_crop2 = Stats(I2).FilledImage;
    %im_bound2 = boundarymask(im_crop2);

    KIN_crop2 = imcrop(KIN_crop,Stats(I2).BoundingBox);


%     figure(1);
%     %subplot(2,2,1); imshow(imoverlay(im_crop,im_bound,'cyan'));
%     subplot(2,2,1); imshow(frame);
%     subplot(2,2,2); imshow(KIN_crop2);
%     subplot(2,2,3); plot(delta);
%     subplot(2,2,4); imshow(im_crop);

    drawnow;
    
    y = KIN_crop2;
    z = im_crop2;
end

function [x_1,y_1,cent_x,cent_y] = fingerTracking(hand_fr, hand_thresh)
    C = detectMinEigenFeatures(hand_thresh);
    x = double(C.Location(:,1));
    y = double(C.Location(:,2));
    K = boundary(x,y,0.1);
    
    L = logical(hand_thresh);
    Stats = regionprops(L,'Area','Centroid');
    
    temp = 1:length(Stats);
    for ii = 1:length(Stats)
        temp(ii) = Stats(ii).Area;
    end
    [~,I] = max(temp);

    cent_x = Stats(I).Centroid(1);
    cent_y = Stats(I).Centroid(2);
    
    jj = 1; K_2 = [];
    for ii = 1:length(K)
        if y(K(ii)) < cent_y
            K_2(jj) = K(ii);
            jj = jj + 1;
        end
    end
    
    %Push up
    for ii = 1:length(K_2)
       for jy = 1:(y(K_2(ii))-1)
           for jx = -5:5
              if (floor(x(K_2(ii)))+jx > 0) && (floor(x(K_2(ii)))+jx < size(hand_thresh,2)) && ((floor(y(K_2(ii)))-jy)>0)
                  if (hand_thresh(floor(y(K_2(ii)))-jy, floor(x(K_2(ii)))+jx) == 1)
                    y(K_2(ii)) = floor(y(K_2(ii)))-jy;
                    x(K_2(ii)) = floor(x(K_2(ii)))+jx;
                  end 
              end
           end
       end
    end
    
    %Remove close points
    jj = 1; K_3 = []; slope = 0;
    for ii = 1:length(K_2)-1
        %slope = (y(KK(ii)) - y(KK(ii+1))) / (x(KK(ii)) - x(KK(ii+1)))
        dist = sqrt(((y(K_2(ii)) - y(K_2(ii+1)))^2) + ((x(K_2(ii)) - x(K_2(ii+1)))^2));
        if (dist > 5)
           if y(K_2(ii)) > y(K_2(ii))
               K_3(jj) = K_2(ii+1);
           else
               K_3(jj) = K_2(ii);
           end
           jj = jj + 1;
        end

    end
    
%     imshow(hand_fr); hold on;
%     plot(x(K_3),y(K_3),'r*');
%     plot(cent_x,cent_y,'r*'); hold off;
%     drawnow;
%     pause(0.5);
    
    x_1 = x(K_3);
    y_1 = y(K_3);
end

function [x_1,y_1] = depthTracking(hand_fr)
    im_bin = imbinarize(hand_fr, 0.65);

    se = strel('disk',2);
    im_morph = imclose(im_bin,se);

    L = logical(im_morph);
    Stats = regionprops(L,'Area','Centroid');

    cen = []; jj = 1;
    for ii = 1:length(Stats)
        if (Stats(ii).Area>10)
            cen(jj,:) = Stats(ii).Centroid;
            jj = jj + 1;
        end
    end

%         subplot(1,2,1); imshow(hand_fr);
%         subplot(1,2,2); imshow(im_morph); hold on;
%         plot(cen(:,1),cen(:,2),'r*'); hold off;
%         pause(0.3);
    x_1 = cen(:,1);
    y_1 = cen(:,2);
end