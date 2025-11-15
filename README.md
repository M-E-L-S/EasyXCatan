# EasyX 多人协作项目开发文档

---

## 项目结构

```
EasyX Project/
│── src/                    # 源代码目录
│   ├── main.cpp            # 主程序入口
│   └── *.cpp                 
│── include/                # 头文件目录
│   └── *.h                 
│── resources/              # 资源文件目录
│   ├── images/             # 图片资源
│   └── sounds/             # 音频资源
├── .gitignore    
├── README.md               
└── CMakeLists.txt          
```

---

## Git 分支规范
```
main              ← 最终发布用（禁止push）
│
└─ dev            ← 开发分支（不可直接push到此分支）
    └── MainGameLoop   ← 自己随便新建分支，完成后合并到 dev 分支
```

### git操作流程

1. 克隆仓库
```bash
git clone https://github.com/M-E-L-S/EasyXCatan.git
cd EasyXCatan
```
2. 每次开发流程
```bash
git checkout dev                     # 切换到开发分支
git pull origin dev                  # 拉取最新的开发分支代码
git checkout -b YourFeatureBranch    # 创建并切换到你自己创建的功能分支
# 进行代码开发
git add .                            # 添加修改的文件
git commit -m "描述你的修改内容"        # 提交修改
git push origin YourFeatureBranch    # 推送你的本地修改到远程仓库
```
3. 创建合并请求（Pull Request）
- 登录 GitHub，回到项目的仓库页面。
- 在 "Pull requests" 选项中，点击"New pull request"
  - base 分支选择 `dev`，compare 分支选择你的功能分支。
  - 务必填写标题和详细描述这次完成的功能，之后 "Create pull request"。
- 等待approval后即可合并至 `dev` 分支。