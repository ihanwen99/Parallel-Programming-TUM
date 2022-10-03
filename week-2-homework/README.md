# Week2 - Homework

这一章节的重点就是  `<thread> <mutex>` 多线程，分为一下几个步骤操作。

1. 引入对应的库

   ```cpp
   #include <thread>
   #include <mutex>
   ```

2. 将变量设置为全局变量，方便多线程进行操作

   ```cpp
   int width = WIDTH;
   int height = HEIGHT;
   int samples = NUM_SAMPLES;
   int depth = DEPTH;
   Vector3 checksum(0, 0, 0);
   const auto aspect_ratio = (float)width / height;
   Camera camera(Vector3(0, 1, 1), Vector3(0, 0, -1), Vector3(0, 1, 0), aspect_ratio, 90, 0.0f, 1.5f);
   std::mutex mtx;
   std::vector<Sphere> spheres;
   auto image_data = static_cast<int *>(malloc(width * height * sizeof(int) * 3));
   ```

3. 创建多线程工作函数

   ```cpp
   void kernel(int low, int high)
   {
       int width = WIDTH;
       int height = HEIGHT;
       int samples = NUM_SAMPLES;
       int depth = DEPTH;
       Vector3 local_checksum(0, 0, 0);
   
       for (int x = low; x < high; x++)
       {
           for (int y = height - 1; y >= 0; y--)
           {
               Vector3 pixel_color(0, 0, 0);
               for (int s = 0; s < samples; s++)
               {
                   auto u = (float)(x + random_float()) / (width - 1);
                   auto v = (float)(y + random_float()) / (height - 1);
                   auto r = get_camera_ray(camera, u, v);
                   pixel_color += trace_ray(r, spheres, depth);
               }
   
               auto output_color = write_color(local_checksum, pixel_color, samples);
   
               int pos = ((height - 1 - y) * width + x) * 3;
               image_data[pos] = output_color.r;
               image_data[pos + 1] = output_color.g;
               image_data[pos + 2] = output_color.b;
           }
       }
   
       mtx.lock();
       checksum += local_checksum;
       mtx.unlock();
   }
   ```

4. `main` 函数中调度多线程:

   - 创建线程
   - 等待线程执行完成

   ```cpp
   #define THREAD_NUM 32
   
   std::thread threads[THREAD_NUM];
   
   for (int thread_id = 0; thread_id < THREAD_NUM; thread_id++)
   {
     int each = width / THREAD_NUM;
     int low = each * thread_id;
     threads[thread_id] = std::thread(kernel, low, (low + each));
   }
   
   for (int thread_id = 0; thread_id < THREAD_NUM; thread_id++)
   {
     threads[thread_id].join();
   }
   ```
