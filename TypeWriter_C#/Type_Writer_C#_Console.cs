// --------------------------------------------------------------------
// // PROGRAMM START
// --------------------------------------------------------------------

// ////////////////////////////////////////////////////////////////////
// //
// //  Programm Autor:  Philip Jähnert
// //  Erstellt am   :  21.12.2015
// //
// ////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------
// // Hauptfunktion menu
// --------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Threading;

class mein_tippprogramm
{
	public static void Main()
	{
		Stopwatch stopWatch = new Stopwatch();
        
        Thread.Sleep(1000);
		
		Console.Clear();
		
		ConsoleKeyInfo keypress;
		
		string input = (System.IO.File.ReadAllText(@"sample_text.txt",System.Text.Encoding.Default)) + " ";
		char[] array = input.ToCharArray();
		string text  = "";
		
		int fehler   = 0;
		int worte	 = 0;
		
		bool fword   = false;
		
		for(int check = 0; check < array.Length-1; check++)
		{
			if (array[check] == '–') array[check] = '-';
			
			if (!(Char.IsWhiteSpace(array[check]))) fword = true;
			
			if (fword == true)
			{
				if (Char.IsWhiteSpace(array[check]))
				{
					worte++;
					fword = false;
				}
			}
		}
		
		Console.WriteLine(input);
		
		int i = 0;
		while(i < array.Length-1)
		{
			Console.BackgroundColor = ConsoleColor.Black;
			Console.ForegroundColor = ConsoleColor.White;
		
			keypress = Console.ReadKey(true);
			
			stopWatch.Start();
			
			if (Char.IsUpper(keypress.KeyChar) && Char.IsUpper(array[i]) && keypress.KeyChar == array[i])
			{
				Console.SetCursorPosition(0,0);
				text += array[i];
				Console.ForegroundColor = ConsoleColor.Green;
				Console.Write(text);
				Console.BackgroundColor = ConsoleColor.Blue;
				Console.ForegroundColor = ConsoleColor.Yellow;
				Console.Write(array[i+1]);
				Console.BackgroundColor = ConsoleColor.Black;
				Console.ForegroundColor = ConsoleColor.White;
				Console.WriteLine(input.Remove(0,2+i));
				i++;
			}
			else if ((!(Char.IsUpper(keypress.KeyChar))) && (!(Char.IsUpper(array[i]))) && keypress.KeyChar == array[i])
			{
				Console.SetCursorPosition(0,0);
				text += array[i];
				Console.ForegroundColor = ConsoleColor.Green;
				Console.Write(text);
				Console.BackgroundColor = ConsoleColor.Blue;
				Console.ForegroundColor = ConsoleColor.Yellow;
				Console.Write(array[i+1]);
				Console.BackgroundColor = ConsoleColor.Black;
				Console.ForegroundColor = ConsoleColor.White;
				Console.WriteLine(input.Remove(0,2+i));
				i++;
			}
			else
			{
				Console.SetCursorPosition(0,0);
				Console.ForegroundColor = ConsoleColor.Green;
				Console.Write(text);
				
				if (((keypress.Modifiers & ConsoleModifiers.Shift) != 0) && ((keypress.Modifiers & ConsoleModifiers.Alt) != 0) && ((keypress.Modifiers & ConsoleModifiers.Control) != 0))
				{
					Console.BackgroundColor = ConsoleColor.Blue;
					Console.ForegroundColor = ConsoleColor.Yellow;
					Console.Write(array[i]);
				}
				else
				{
					Console.BackgroundColor = ConsoleColor.Red;
					Console.ForegroundColor = ConsoleColor.White;
					Console.Write(array[i]);
					fehler++;
				}
				Console.BackgroundColor = ConsoleColor.Black;
				Console.ForegroundColor = ConsoleColor.White;
				Console.WriteLine(input.Remove(0,1+i));
			}
		}
		
		stopWatch.Stop();
		
		Console.ForegroundColor = ConsoleColor.White;
		Console.WriteLine("\nIhre abgetippte Zeit\t= {0:00}:{1:00}:{2:00}.{3:00} h\n", stopWatch.Elapsed.Hours, stopWatch.Elapsed.Minutes, stopWatch.Elapsed.Seconds, stopWatch.Elapsed.Milliseconds / 10);
		Console.WriteLine("Anschläge pro Minute\t= {0:f2}", ((array.Length-1)/Convert.ToDouble(stopWatch.Elapsed.Seconds))*60);
		Console.ForegroundColor = ConsoleColor.Green;
		Console.WriteLine("Anschläge pro Minute\t= {0:f2}", (((array.Length-1)-fehler)/Convert.ToDouble(stopWatch.Elapsed.Seconds))*60);
		Console.WriteLine("Anschläge pro Sekunde\t= {0:f2}\n", ((array.Length-1)-fehler)/Convert.ToDouble(stopWatch.Elapsed.Seconds));
		Console.ForegroundColor = ConsoleColor.White;
		Console.WriteLine("Gesamte Zeichen\t\t= {0}", array.Length-1);
		Console.WriteLine("Gesamte Wörter\t\t= {0}", worte+1);
		Console.WriteLine("Gesamte Fehler\t\t= {0}\n", fehler);
		Console.WriteLine("Wörter pro Minute (WPM)\t= {0:f2}\n", ((worte+1)/Convert.ToDouble(stopWatch.Elapsed.Seconds))*60);
		
		Console.ReadKey();
		Console.Clear();
	}
}

// --------------------------------------------------------------------
// // PROGRAMM ENDE
// --------------------------------------------------------------------