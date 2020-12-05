import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np
import gym
import sys

global id_count
id_ = 0


def record(x):
    global id_
    id_ += 1
    f = open('addresses-locality.txt', 'a')
    f.write(str(id(x) % 65536) + '\n')
    f.close()
    print('id_ = ', id_)
    if id_ == 10000:
        sys.exit(0)

# 超参数
BATCH_SIZE = 32
LR = 0.01
EPSILON = 0.9  # 随机选取的概率，如果概率小于这个随机数，就采取greedy的行为
GAMMA = 0.9
TARGET_REPLACE_ITER = 100
MEMORY_CAPACITY = 2000
# 导入openAI gym实验的模拟场所，'CartPole-v0'表示倒立摆的实验
env = gym.make('CartPole-v0')
env = env.unwrapped
N_ACTIONS = env.action_space.n  # 小车的动作
N_STATES = env.observation_space.shape[0]  # 实验环境的状态
ENV_A_SHAPE = 0 if isinstance(env.action_space.sample(), int) else env.action_space.sample().shape


class Net(nn.Module):
    def __init__(self, ):
        super(Net, self).__init__()
        record(super(Net, self).__init__())
        self.fc1 = nn.Linear(N_STATES, 50)
        record(self.fc1)
        self.fc1.weight.data.normal_(0, 0.1)
        record(self.fc1.weight.data.normal_(0, 0.1))
        self.out = nn.Linear(50, N_ACTIONS)
        record(self.out)
        self.out.weight.data.normal_(0, 0.1)
        record(self.out.weight.data.normal_(0, 0.1))

    def forward(self, x):
        x = self.fc1(x)
        record(x)
        x = F.relu(x)
        record(x)
        actions_value = self.out(x)
        record(actions_value)
        return actions_value


class DQN(object):
    def __init__(self):
        # DQN是Q-Leaarning的一种方法，但是有两个神经网络，一个是eval_net一个是target_net
        # 两个神经网络相同，参数不同，是不是把eval_net的参数转化成target_net的参数，产生延迟的效果
        self.eval_net, self.target_net = Net(), Net()
        record(self.eval_net)
        record(self.target_net)
        self.learn_step_counter = 0  # 学习步数计数器
        record(self.learn_step_counter)
        self.memory_counter = 0  # 记忆库中位值的计数器
        record(self.memory_counter)
        self.memory = np.zeros((MEMORY_CAPACITY, N_STATES * 2 + 2))  # 初始化记忆库
        record(self.memory)
        # 记忆库初始化为全0，存储两个state的数值加上一个a(action)和一个r(reward)的数值
        self.optimizer = torch.optim.Adam(self.eval_net.parameters(), lr=LR)
        record(self.optimizer)
        self.loss_func = nn.MSELoss()  # 优化器和损失函数
        record(self.loss_func)

    # 接收环境中的观测值，并采取动作
    def choose_action(self, x):
        # x为观测值
        x = torch.unsqueeze(torch.FloatTensor(x), 0)
        record(x)
        if np.random.uniform() < EPSILON:
            # 随机值得到的数有百分之九十的可能性<0.9,所以该if成立的几率是90%
            # 90%的情况下采取actions_value高的作为最终动作
            actions_value = self.eval_net.forward(x)
            record(actions_value)
            action = torch.max(actions_value, 1)[1].data.numpy()
            record(action)
            action = action[0] if ENV_A_SHAPE == 0 else action.reshape(ENV_A_SHAPE)  # return the argmax index
            record(action)
        else:
            # 其他10%采取随机选取动作
            action = np.random.randint(0, N_ACTIONS)  # 从动作中选一个动作
            record(action)
            action = action if ENV_A_SHAPE == 0 else action.reshape(ENV_A_SHAPE)
            record(action)
        return action

        # 记忆库，存储之前的记忆，学习之前的记忆库里的东西

    def store_transition(self, s, a, r, s_):
        transition = np.hstack((s, [a, r], s_))
        record(transition)
        # 如果记忆库满了, 就覆盖老数据
        index = self.memory_counter % MEMORY_CAPACITY
        record(index)
        self.memory[index, :] = transition
        record(self.memory[index, :])
        self.memory_counter += 1
        record(self.memory_counter)

    def Learn(self):
        # target net 参数更新,每隔TARGET_REPLACE_ITE更新一下
        if self.learn_step_counter % TARGET_REPLACE_ITER == 0:
            self.target_net.load_state_dict(self.eval_net.state_dict())
        self.learn_step_counter += 1
        record(self.learn_step_counter)
        # targetnet是时不时更新一下，evalnet是每一步都更新

        # 抽取记忆库中的批数据
        sample_index = np.random.choice(MEMORY_CAPACITY, BATCH_SIZE)
        record(sample_index)
        b_memory = self.memory[sample_index, :]
        record(b_memory)
        # 打包记忆，分开保存进b_s，b_a，b_r，b_s
        b_s = torch.FloatTensor(b_memory[:, :N_STATES])
        record(b_s)
        b_a = torch.LongTensor(b_memory[:, N_STATES:N_STATES + 1].astype(int))
        record(b_a)
        b_r = torch.FloatTensor(b_memory[:, N_STATES + 1:N_STATES + 2])
        record(b_r)
        b_s_ = torch.FloatTensor(b_memory[:, -N_STATES:])
        record(b_s_)

        # 针对做过的动作b_a, 来选 q_eval 的值, (q_eval 原本有所有动作的值)
        q_eval = self.eval_net(b_s).gather(1, b_a)  # shape (batch, 1)
        record(q_eval)
        q_next = self.target_net(b_s_).detach()  # q_next 不进行反向传递误差, 所以 detach
        record(q_next)
        q_target = b_r + GAMMA * q_next.max(1)[0]  # shape (batch, 1)
        record(q_target)
        loss = self.loss_func(q_eval, q_target)
        record(loss)

        # 计算, 更新 eval net
        self.optimizer.zero_grad()
        loss.backward()  # 误差反向传播
        self.optimizer.step()


dqn = DQN()

print('\nCollection experience...')
for i_episode in range(400):
    s = env.reset()  # 得到环境的反馈，现在的状态
    record(s)
    ep_r = 0
    record(ep_r)
    while True:
        env.render()  # 环境渲染，可以看到屏幕上的环境
        a = dqn.choose_action(s)  # 根据dqn来接受现在的状态，得到一个行为
        record(a)
        s_, r, done, info = env.step(a)  # 根据环境的行为，给出一个反馈
        record(s_)
        record(r)
        record(done)
        record(info)

        # 修改 reward, 使 DQN 快速学习
        x, x_dot, theta, theta_dot = s_
        record(x)
        record(x_dot)
        record(theta)
        record(theta_dot)
        r1 = (env.x_threshold - abs(x)) / env.x_threshold - 0.8
        record(r1)
        r2 = (env.theta_threshold_radians - abs(theta)) / env.theta_threshold_radians - 0.5
        record(r2)
        r = r1 + r2
        record(r)

        dqn.store_transition(s, a, r, s_)  # dqn存储现在的状态，行为，反馈，和环境导引的下一个状态

        ep_r += r
        record(ep_r)

        if dqn.memory_counter > MEMORY_CAPACITY:
            dqn.learn()
            if done:
                print('Ep: ', i_episode,
                      '| Ep_r: ', round(ep_r, 2))

        if done:
            break

        s = s_  # 现在的状态赋值到下一个状态上去
        record(s)