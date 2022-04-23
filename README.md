# README #

[1分钟视频演示(P2)](https://www.bilibili.com/video/BV133411M71S)

#### 完成内容：

- Actor：使用c++结合蓝图完成人物，枪，手雷和一个actor自动生成器的编写。纯蓝图完成了游戏中需要用到的保龄球actor，踩踏按钮，计分板，靶子，来回移动的浮板，用于演示景深的pawn。
  - 人物实现了自由视角，拿枪，拿雷三种状态的切换，切换会引起umg，动作蓝图，摄像机，视角的改变。枪实现了弹药换弹（无cd，下个作业会完善），通知umg更新子弹。手雷实现了抛物线弹道预测（tick)。其他用于完善游戏逻辑的actor也都实现了一定程度的自定义配置（指在实例中修改参数），并且可以在不同情况下复用。在焦距测试的小场景下做了个希区柯克变焦的效果（个人小兴趣）。
- 动画：完成了持枪在腰上和举枪两套混合空间，以及对应的俯仰角瞄准偏移1D。丢手雷的动作没有加上，主要是想放在最后完成动画，但时间有限，所以动画上面很多细节没有处理。
- 材质：完成了枪的炫彩特效，为按钮，靶子和弹孔做了简单的材质。

#### 缺点：

- 由于刚接触使用UE c++，在人物角色类里写了太多功能，~~最好将角色状态切换交给player controler~~ (有待进一步思考)，将武器和人物解耦合。第三次作业会更加仔细考虑。
- 动画方面，一是由于动画包的原因，八方向移动的效果不是很好。开火的时候直接播放了蒙太奇导致瞄准偏移和射击动画出现闪现，应该在动画蓝图中调整混合。手雷的动作也没有添加，导致手雷模式人物挡住了抛物线，不太舒服。因为别的想法做了很久而没时间完善动画，将会在最后一次作业更加完善。
- 没有适配安卓，因为时间不够，这次作业我完成的内容有点多和杂，需要许多按钮，电脑方便操作。并且动画也还没做完，适配起来要许多时间应该。



