import numpy as np
import tensorflow as tf
from tensorflow.keras.layers import Dense, Input, Conv2D, Flatten, BatchNormalization, Activation

from typing import Final

def resBlock(x): 
    Fx = Conv2D(filters=256, kernel_size=(3, 3), padding='same')(x)
    Fx = BatchNormalization()(Fx)
    Fx = Activation('relu')(Fx)
    Fx = Conv2D(filters=256, kernel_size=(3, 3), padding='same')(Fx)
    Fx = BatchNormalization()(Fx)
    Hx = tf.keras.layers.add([x, Fx])
    Hx = Activation('relu')(Hx)
    return Hx

class NetArchitecture:
    def __init__(self, stateTensor: tuple[int, int, int], actionSize: int, **kwargs) -> None:
        self.args: Final = kwargs
        self.input_shape: Final = stateTensor
        self.action_size: Final = actionSize
        
        # initialize the model
        x = Input(shape=self.input_shape)

        # first convolutional block
        conv1 = Conv2D(filters=256, kernel_size=(3, 3), padding='same')(x)
        norm1 = BatchNormalization()(conv1)
        relu1 = Activation('relu')(norm1)

        # pass through 39 residual blocks
        res_out = relu1
        for _ in range(39):
            res_out = resBlock(res_out)

        # policy head
        convPolicy = Conv2D(filters=2, kernel_size=(1, 1), padding='same')(res_out)
        normPolicy = BatchNormalization()(convPolicy)
        reluPolicy = Activation('relu')(normPolicy)
        flatPolicy = Flatten()(reluPolicy)
        self.pi = Dense(self.action_size, activation='softmax', name='pi')(flatPolicy)

        # value head
        convValue = Conv2D(filters=1, kernel_size=(1, 1), padding='same')(res_out)
        normValue = BatchNormalization()(convValue)
        reluValue = Activation('relu')(normValue)
        flatValue = Flatten()(reluValue)
        denseValue = Dense(256, activation='relu')(flatValue)
        self.v = Dense(1, activation='tanh', name='v')(denseValue)

        # output results
        self.model = tf.keras.Model(inputs=x, outputs=[self.pi, self.v])
        self.model.compile(loss=['categorical_crossentropy', 'mean_squared_error'], optimizer=tf.keras.optimizers.Adam(learning_rate=self.args['lr']))

    def predict(self, state: np.ndarray) -> tuple[np.ndarray, float]:
        state = state[np.newaxis, :, :, :]  # add batch dimension
        pi, v = self.model.predict(state)
        return pi[0], v[0][0]
    
    def train(self, state: np.ndarray, target_pi: np.ndarray, target_v: np.ndarray) -> None:
        state = state.astype(np.float32)
        target_pi = target_pi.astype(np.float32)
        target_v = target_v.astype(np.float32)
        self.model.fit(state, [target_pi, target_v], batch_size=self.args['batch_size'], epochs=self.args['epochs'])

    def save_model(self, filepath: str) -> None:
        self.model.save_weights(filepath)

    def load_model(self, filepath: str) -> None:
        self.model.load_weights(filepath)