public const string _dllPath = @"C:\Users\Administrator\Desktop\demo\WinFormsApp1206\WinFormsApp1\Debug\ScreenCloneDx9.dll";
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        // 数据初始化
        public static extern void DisplayCloneInit();
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        // 根据显示器名称获取关联显卡索引
        public static extern int GetAdapterDevice(string deviceName);
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        public static extern void AddCloneInfo(int adapter, int ScreenPosX, int ScreenPosY, int ScreenWidth, int ScreenHeight,
                                               int ScreenActualWidth, int ScreenActualHeight, int type);
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        // 业务执行
        public static extern void StartClone();
        [DllImport(_dllPath, CallingConvention = CallingConvention.StdCall)]
        // 退出
        public static extern void QuitClone();

        bool _IsRunning = false;
        private static ScreenClone? _Instance = null;
        Task CloneTask = new Task(new Action(() => {; }));
        List<string> Source = new List<string>();
        List<string> Dest = new List<string>();

        public static ScreenClone GetInstance()
        {
            if (_Instance == null)
            {
                _Instance = new ScreenClone();
            }
            return _Instance;
        }

        private void GetScreenInfo()
        {
            foreach (Screen screen in Screen.AllScreens)
            {
                Rectangle bounds = screen.Bounds;
                string deviceName = screen.DeviceName;
                byte[] aa = System.Text.Encoding.UTF8.GetBytes(screen.DeviceName);
                //string deviceName = "11111";
                int adapter = GetAdapterDevice(deviceName);
                int a = 0;
            }
        }

        private void DoWork()
        {
            Console.WriteLine("C# ----------Dowork");
            int threadID = Thread.CurrentThread.ManagedThreadId;
            Console.WriteLine("C# thread ID: " + threadID);
            DisplayCloneInit();
            AddCloneInfo(0, 0, 0, 1920, 1080, 1920, 1080, 0);
            AddCloneInfo(1, 4480, 0, 1600, 900, 1600, 900, 1);
            AddCloneInfo(2, 1920, 0, 2560, 1440, 2560, 1440, 0);
            StartClone();
            task.Wait();
            Console.WriteLine("Task state: " + task.Status);
            //Thread.CurrentThread.Abort();
        }

        public bool EnableScreenClone()
        {
            GetScreenInfo();
            try
            {
                if (!_IsRunning)
                {
                    task = Task.Factory.StartNew(DoWork); // 可行
                }
            catch (Exception e)
            {
                MessageBox.Show("Enable Screen Clone Error：" + e.Message);
                return _IsRunning;
            }
            _IsRunning = true;
            return _IsRunning;
        }
