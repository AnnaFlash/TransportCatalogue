#include "svg.h"

namespace svg {
    std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cup)
    {
        if (line_cup == StrokeLineCap::BUTT) {
            out << "butt";
        }
        if (line_cup == StrokeLineCap::ROUND) {
            out << "round";
        }
        if (line_cup == StrokeLineCap::SQUARE) {
            out << "square";
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_cup)
    {
        if (line_cup == StrokeLineJoin::ARCS) {
            out << "arcs";
        }
        if (line_cup == StrokeLineJoin::BEVEL) {
            out << "bevel";
        }
        if (line_cup == StrokeLineJoin::MITER) {
            out << "miter";
        }
        if (line_cup == StrokeLineJoin::MITER_CLIP) {
            out << "miter-clip";
        }
        if (line_cup == StrokeLineJoin::ROUND) {
            out << "round";
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, Rgb rgb_)
    {
        out << "rgb(" << unsigned(rgb_.red) << ',' << unsigned(rgb_.green) << ',' << unsigned(rgb_.blue) << ')';
        return out;
    }


    std::ostream& operator<<(std::ostream& out, Rgba rgba_)
    {
        out << "rgba(" << unsigned(rgba_.red) << ',' << unsigned(rgba_.green) << ',' << unsigned(rgba_.blue) << ',' << rgba_.opacity << ')';
        return out;
    }

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point)
    {
        // TODO: вставьте здесь оператор return
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (auto it = points_.begin(); it != points_.end(); it++) {
            if (it == points_.begin()) {
                out << it->x << ","sv << it->y;
            }
            else {
                out << " " << it->x << ","sv << it->y;
            }
        }
        out << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    Text& Text::SetPosition(Point pos)
    {
        // TODO: вставьте здесь оператор return
        pos_ = std::move(pos);
        return *this;
    }

    Text& Text::SetOffset(Point offset)
    {
        // TODO: вставьте здесь оператор return
        offset_ = std::move(offset);
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size)
    {
        // TODO: вставьте здесь оператор return
        size_ = std::move(size);
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family)
    {
        // TODO: вставьте здесь оператор return
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight)
    {
        // TODO: вставьте здесь оператор return
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data)
    {
        // TODO: вставьте здесь оператор return
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << " x=\""sv << this->pos_.x << "\" y=\""sv << this->pos_.y << "\""sv
            << " dx=\""sv << this->offset_.x << "\" dy=\""sv << this->offset_.y << "\""sv
            << " font-size=\"" << this->size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << this->font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << this->font_weight_ << "\""sv;
        }
        out << ">"sv << this->data_ << "</text>"sv;
    }

    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        objects_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        RenderContext context(out, 2, 2);

        context.out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        context.out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

        for (const auto& obj : objects_) {
            obj->Render(context);
        }

        context.out << "</svg>"sv;
    }
}  // namespace svg