using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Newtonsoft.Json.Linq;

namespace WebDev.Controllers
{
    [Route("")]
    [ApiController]
    public class ValuesController : ControllerBase
    {
        // GET api/values
        [HttpGet("d")]
        //public ActionResult<IEnumerable<string>> Get()
        public ActionResult<Dictionary<string,int>> GetData()
        {
            //return new string[] { "value1", "value2" };
            return new Dictionary<string, int>() { { "I0", 0 }, { "I1", 0 }, { "I2", 1 }, { "I3", new Random().Next(65535) } };
        }

        // GET api/values
        [HttpGet("c")]
        //public ActionResult<IEnumerable<string>> Get()
        public ActionResult<Dictionary<string, int>> GetConfig()
        {
            //return new string[] { "value1", "value2" };
            return new Dictionary<string, int>() { { "C0", 0 }, { "C1", 0 }, { "C2", 1 }, { "C3", 1 } };
        }

        // POST api/values
        [HttpPost("u")]
        public void PostUpdate([FromBody] JObject value)
        {
        }


    }
}
