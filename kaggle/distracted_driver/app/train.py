#!/usr/bin/env python
# coding=utf-8

import os
import cv2
import glob
import datetime
import logging

import numpy as np

from keras.utils import np_utils
from keras.models import Sequential
from keras.layers.core import Dense, Dropout, Activation, Flatten
from keras.layers.convolutional import Convolution2D, MaxPooling2D

from ml_utils import split_cv, save_model


current_dir = os.path.dirname(os.path.abspath(__file__))
log_path = os.path.join(current_dir, os.path.pardir, 'log', datetime.date.today().strftime('%Y%m%d') + '.log')

logger = logging.getLogger('train')
logger.setLevel(logging.DEBUG)
fh = logging.FileHandler(log_path)
ch = logging.StreamHandler()
fh.setLevel(logging.DEBUG)
ch.setLevel(logging.DEBUG)
formatter = logging.Formatter('[%(asctime)s][%(name)s][%(levelname)s]: %(message)s')
fh.setFormatter(formatter)
ch.setFormatter(formatter)
logger.addHandler(fh)
logger.addHandler(ch)

def load_img(img_path, img_rows, img_cols):
    # read image to a grayscale buffer
    # print img_path
    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    rows, cols = img.shape
    # print rows, cols

    # print img_rows, img_cols

    resized = cv2.resize(img, (img_cols, img_rows), interpolation = cv2.INTER_CUBIC);
    return resized

def load_train(img_rows, img_cols):
  X_train = []
  Y_train = []
  i = 0
  for j in range(10):
    path = os.path.join('/workshop2/data/driver-distraction', 'train', 'c' + str(j), '*.jpg')
    files = glob.glob(path)
    for fl in files:
      i += 1
      # print fl
      img = load_img(fl, img_rows, img_cols)
      X_train.append(img)
      Y_train.append(j)
  logger.info("%d samples in total" % (i))
  return X_train, Y_train

img_rows = 96
img_cols = 128

batch_size = 64
nb_classes = 10
nb_epoch = 2
nb_filters = 32
nb_pool = 2
nb_conv = 3

if __name__ == "__main__":
  logger.info("start training")

  # read training data
  train_data, train_labels = load_train(img_rows, img_cols)
  train_data = np.array(train_data, dtype = np.uint8)
  train_labels = np.array(train_labels, dtype = np.uint8)
  train_data = train_data.reshape(train_data.shape[0], 1, img_rows, img_cols)
  train_labels = np_utils.to_categorical(train_labels, nb_classes)
  train_data = train_data.astype('float32')
  train_data /= 127.5
  train_data -= 1.0
  logger.info("read training data complete")

  # split for cross validation
  train, train_label, validation, validation_label = split_cv(train_data, train_labels)
  logger.info("data split complete")

  # build stacking layers
  model = Sequential()
  model.add(Convolution2D(nb_filters, nb_conv, nb_conv, border_mode = 'valid', input_shape = (1, img_rows, img_cols)))
  model.add(Activation('relu'))

  model.add(Convolution2D(nb_filters, nb_conv, nb_conv))
  model.add(Activation('relu'))

  model.add(MaxPooling2D(pool_size=(nb_pool, nb_pool)))
  model.add(Dropout(0.25))

  model.add(Flatten())
  model.add(Dense(128))
  model.add(Activation('relu'))
  model.add(Dropout(0.5))
  model.add(Dense(nb_classes))
  model.add(Activation('softmax'))

  model.compile(loss = 'categorical_crossentropy', optimizer = 'adadelta')

  model.fit(train, train_label, batch_size = batch_size, nb_epoch = nb_epoch, verbose = 1, validation_data = (validation, validation_label))

  logger.info("model training complete")

  score = model.evaluate(validation, validation_label, verbose = 0)

  logger.info("validation score: %f" % (score))

  save_model(model)
  logger.info("model saved")

