using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EightCylinders;


namespace BoreScope
{
    class Program
    {
        static void Main(string[] args)
        {
            JSEnvironment env = new JSEnvironment();
            Console.WriteLine(env.Test());

            Console.WriteLine(env.Eval("a = 6;\nb = 12;\n(a+b).toString();"));

            try
            {
                Console.WriteLine(env.Eval("(function() { try { throw 'No Real Errors Here'; } catch (err) { return err; } })();"));
            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception Caught");
                Console.WriteLine(ex.Message);
            }

        }
    }
}
