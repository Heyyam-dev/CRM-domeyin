using Microsoft.AspNetCore.Mvc;
using XaApi.Models;
using XaApi.Services;

namespace XaApi.Controllers;

[ApiController]
[Route("api/[controller]")]
public class ContactController : ControllerBase
{
    private readonly IContactService _contactService;

    public ContactController(IContactService contactService)
    {
        _contactService = contactService;
    }

    [HttpGet]
    public async Task<IActionResult> GetAll()
    {
        var contacts = await _contactService.GetAllAsync();
        return Ok(contacts);
    }

    [HttpPost]
    public async Task<IActionResult> Create([FromBody] Contact contact)
    {
        if (!ModelState.IsValid)
            return BadRequest(ModelState);

        await _contactService.CreateAsync(contact);
        return Ok(new { message = "Müraciət qəbul edildi." });
    }
}
