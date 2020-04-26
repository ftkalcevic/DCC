using System;
using System.IO;
using System.IO.Compression;
using System.Text;
using NUglify;
using NUglify.Html;

namespace CompressHTML
{
    class HexStream : System.IO.Stream
    {
        private int length;
        private Stream output;
        private StringBuilder sb = new StringBuilder();

        public HexStream(Stream output) 
        {
            this.output = output;
            length = 0;
        }

        public override bool CanRead => false;

        public override bool CanSeek => false;

        public override bool CanWrite => true;

        public override long Length => throw new NotImplementedException();

        public override long Position { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public override void Flush()
        {
            throw new NotImplementedException();
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            int len = Math.Min(count - offset, sb.Length);
            byte[] data = Encoding.UTF8.GetBytes(sb.ToString());

            for (int i = 0; i < len; i++)
                buffer[i + offset] = data[i];
            return len;
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            for (int i = offset; i < count; i++ )
            {
                length++;
                byte b = buffer[i];
                string s = $"0x{b:X2},";
                if ((length & 0x0F) == 0)
                    s += "\n";
                if (output != null)
                {
                    byte[] data = System.Text.Encoding.ASCII.GetBytes(s);
                    output.Write(data, 0, data.Length);
                }
                else
                {
                    sb.Append(s);
                }
            }
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                System.Console.Error.WriteLine($"Usage: CompressHTML  inputfile outputfile");
                Environment.Exit(1);
            }

            string inputFileName = args[0];
            string outputFileName = args[1];
            if (!File.Exists(inputFileName))
            {
                System.Console.Error.WriteLine($"File '{inputFileName}' doesn't exist.");
                Environment.Exit(1);
            }

            //using (FileStream originalFileStream = new FileStream(inputFileName, FileMode.Open))
            //{
            //    StreamReader sr = new StreamReader(originalFileStream);
            //    string s = sr.ReadToEnd();
            //    //string m = Uglify.Html(s).ToString();

            //    MemoryStream ms = new MemoryStream(Encoding.UTF8.GetBytes(s));


            //    MemoryStream cms = new MemoryStream();

            //    //using (var compressionStream = new GZipStream(cms, CompressionMode.Compress))
            //    using (var compressionStream = new DeflateStream(cms, CompressionMode.Compress))
            //    //using (var compressionStream = new BrotliStream(cms, CompressionMode.Compress)) // Brotli is best, but only works under HTTPS
            //    {
            //        ms.CopyTo(compressionStream);

            //        using (FileStream compressedFileStream = File.Create(outputFileName))
            //        {
            //                compressionStream.CopyTo(hexStream);
            //            }
            //        }

            //    }
            //}

            using (FileStream compressedFileStream = File.Create(outputFileName))
            {
                using (HexStream hexStream = new HexStream(compressedFileStream))
                {
                    //using (var compressionStream = new GZipStream(hexStream, CompressionMode.Compress))
                    using (var compressionStream = new DeflateStream(hexStream, CompressionMode.Compress))
                    //using (var compressionStream = new BrotliStream(hexStream, CompressionMode.Compress)) // Brotli is best, but only works under HTTPS
                    {
                        using (FileStream originalFileStream = new FileStream(inputFileName, FileMode.Open))
                        {
                            StreamReader sr = new StreamReader(originalFileStream);
                            string s = sr.ReadToEnd();
                            string m = Uglify.Html(s).ToString();

                            MemoryStream ms = new MemoryStream(Encoding.UTF8.GetBytes(m));
                            ms.Seek(0, SeekOrigin.Begin);

                            ms.CopyTo(compressionStream);
                        }
                    }
                }
            }
        

        //using (FileStream compressedFileStream = File.Create(outputFileName))
        //{
        //    using (HexStream hexStream = new HexStream(compressedFileStream))
        //    {
        //        //using (var compressionStream = new GZipStream(hexStream, CompressionMode.Compress))
        //        using (var compressionStream = new DeflateStream(hexStream, CompressionMode.Compress))
        //        //using (var compressionStream = new BrotliStream(hexStream, CompressionMode.Compress)) // Brotli is best, but only works under HTTPS
        //        {
        //            using (FileStream originalFileStream = new FileStream(inputFileName, FileMode.Open))
        //            {
        //                originalFileStream.CopyTo(compressionStream);
        //            }
        //        }
        //    }
        //}

        //    long fileLength, compressedLength;
        //    fileLength = (new FileInfo(inputFileName)).Length;
        //    using (FileStream compressedFileStream = File.Create(outputFileName))
        //    {
        //        using (HexStream hexStream = new HexStream(compressedFileStream))
        //        {
        //            using (var compressionStream = new GZipStream(hexStream, CompressionMode.Compress))
        //            //using (var compressionStream = new DeflateStream(hexStream, CompressionMode.Compress))
        //            //using (var compressionStream = new BrotliStream(hexStream, CompressionMode.Compress)) // Brotli is best, but only works under HTTPS
        //            {
        //                using (FileStream originalFileStream = new FileStream(inputFileName, FileMode.Open))
        //                {
        //                    //StreamReader sr = new StreamReader(originalFileStream);
        //                    //string s = sr.ReadToEnd();
        //                    //string m = Uglify.Html(s).ToString();
        //                    //System.Console.WriteLine($"Minified {s.Length} to {m.Length} {(double)m.Length / (double)s.Length * 100}%");
        //                    //MemoryStream ms = new MemoryStream(Encoding.ASCII.GetBytes(s));
        //                    //ms.CopyTo(compressionStream);
        //                    compressedLength = 0;// compressionStream.Length;
        //                    originalFileStream.CopyTo(compressionStream);
        //                }
        //            }
        //        }
        //    }
        //    System.Console.WriteLine($"Compressed {fileLength} to {compressedLength} {(double)compressedLength / (double)fileLength * 100}%");
    }
}
}
