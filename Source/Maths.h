/*
    Author: miha53cevic
    Github: https://github.com/miha53cevic
    Date: 5.9.2019.
*/

#pragma once
#include <stdio.h>
#include <math.h>

#include "Vec2.h"

enum EdgeType
{
    VERTICAL_LINE, HORIZONTAL_LINE, NORMAL_LINE
};

struct Edge
{
    Edge() {}
    Edge(vec2f a, vec2f b)
    {
        this->a = a;
        this->b = b;

        calculateEquation();
    }

    void Initialize(vec2f a, vec2f b)
    {
        this->a = a;
        this->b = b;

        calculateEquation();
    }

    void calculateEquation()
    {
        // HORIZONTAL_LINE
        // y = 5;
        // k = 0;
        if (b.y - a.y == 0)
        {
            k = 0;
            l = a.y;
            type = EdgeType::HORIZONTAL_LINE;
        }
        // VERTICAL_LINE
        // x = 5;
        // k = infinity
        else if (b.x - a.x == 0)
        {
            k = INFINITY;
            // Pomak na y-osi je undefined, ali spremam u njega pomak na x-osi
            l = a.x;
            type = EdgeType::VERTICAL_LINE;
        }
        // NORMAL_LINE
        else
        {
            k = (b.y - a.y) / (b.x - a.x);
            l = a.y - (k * a.x);
            type = EdgeType::NORMAL_LINE;
        }

        // Calculate length
        length = sqrtf(powf(b.x - a.x, 2) + powf(b.y - a.y, 2));

        //printf("k = %f, l = %f, Type = %d, Length = %f\n", k, l, type, length);
    }
    Edge okomica()
    {
        // Calculate center point sx, sy
        float sx = (this->a.x + this->b.x) / 2;
        float sy = (this->a.y + this->b.y) / 2;

        // Okomica
        Edge okomit;

        // HORIZONTAL LINE
        if (this->type == EdgeType::HORIZONTAL_LINE)
        {
            vec2f a(sx, sy - this->length / 2);
            vec2f b(sx, sy + this->length / 2);
            okomit.Initialize(a, b);
        }
        // VERTICAL LINE
        else if (this->type == EdgeType::VERTICAL_LINE)
        {
            vec2f a(sx - this->length / 2, sy);
            vec2f b(sx + this->length / 2, sy);
            okomit.Initialize(a, b);
        }
        // NORMAL LINE
        else
        {
            // Obrni -k, izracunaj zatim l
            // Izracunaj dvije tocke na tom pravcu prema jednadzbi y = kx + l
            float k2 = -1 / this->k;
            float l2 = sy - (k2 * sx);

            // Prva tocka
            float x2 = sx - this->length / 2;
            float y2 = k2 * x2 + l2;

            // Druga tocka
            float x3 = sx + this->length / 2;
            float y3 = k2 * x3 + l2;

            vec2f a(x2, y2);
            vec2f b(x3, y3);
            okomit.Initialize(a, b);
        }

        return okomit;
    }

    vec2f sjeciste(Edge rb)
    {
        vec2f p;

        // P.S. kod vertikalne linije spremam pomak na x osi u l
        // l je inace undefined jer je k = infinity

        // HORIZONTAL LINE & VERTICAL
        if (this->type == EdgeType::HORIZONTAL_LINE && rb.type == EdgeType::VERTICAL_LINE)
        {
            p.x = rb.l;
            p.y = this->l;
        }
        // VERTICAL LINE & HORIZONTAL
        else if (this->type == EdgeType::VERTICAL_LINE && rb.type == EdgeType::HORIZONTAL_LINE)
        {
            p.x = this->l;
            p.y = rb.l;
        }
        // HORIZONTAL & NORMAL
        else if (this->type == EdgeType::HORIZONTAL_LINE && rb.type == EdgeType::NORMAL_LINE)
        {
            p.y = this->l;
            p.x = (p.y - rb.l) / rb.k;
        }
        // NORMAL & HORIZONTAL
        else if (this->type == EdgeType::NORMAL_LINE && rb.type == EdgeType::HORIZONTAL_LINE)
        {
            p.y = rb.l;
            p.x = (p.y - this->l) / this->k;
        }
        // VERTICAL & NORMAL
        else if (this->type == EdgeType::VERTICAL_LINE && rb.type == EdgeType::NORMAL_LINE)
        {
            p.x = this->l;
            p.y = rb.k * p.x + rb.l;
        }
        // NORMAL & VERTICAL
        else if (this->type == EdgeType::NORMAL_LINE && rb.type == EdgeType::VERTICAL_LINE)
        {
            p.x = rb.l;
            p.y = this->k * p.x + this->l;
        }
        // Both NORMAL LINES
        else if (this->type == EdgeType::NORMAL_LINE && rb.type == EdgeType::NORMAL_LINE)
        {
            // Sustav dviju jednad�bi pravca
            // x = (l2 - l1) / (k1 - k2)
            // y = k1x + l1 || y = k2x + l2
            p.x = (rb.l - this->l) / (this->k - rb.k);
            p.y = this->k * p.x + this->l;
        }
        // HORIZONTAL & HORIZONTAL || VERTICAL & VERTICAL
        else
        {
            // Error return infinity
            p.x = INFINITY;
            p.y = INFINITY;

            printf("Horizontal & Horizontal or Vertical & Vertical operation!\n");
        }

        return p;
    }

    bool equals(Edge othr)
    {
        if (a.x == othr.a.x && a.y == othr.a.y && b.x == othr.b.x && b.y == othr.b.y)
            return true;
        else if (a.x == othr.b.x && a.y == othr.b.y && b.x == othr.a.x && b.y == othr.a.y)
            return true;
        else return false;
    }

    vec2f a, b;
    float k, l;
    float length;

    EdgeType type;
};

struct Triangle
{
    Triangle() {}
    Triangle(vec2f a, vec2f b, vec2f c)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;

        edges[0] = Edge(a, b);
        edges[1] = Edge(b, c);
        edges[2] = Edge(c, a);
    }
    Triangle(Edge a, Edge b, Edge c)
    {
        edges[0] = a;
        edges[1] = b;
        edges[2] = c;

        points[0] = a.sjeciste(b);
        points[1] = b.sjeciste(c);
        points[2] = c.sjeciste(a);
    }

    void Initialize(vec2f a, vec2f b, vec2f c)
    {
        points[0] = a;
        points[1] = b;
        points[2] = c;

        edges[0] = Edge(a, b);
        edges[1] = Edge(b, c);
        edges[2] = Edge(c, a);
    }
    void Initialize(Edge a, Edge b, Edge c)
    {
        edges[0] = a;
        edges[1] = b;
        edges[2] = c;

        points[0] = a.sjeciste(b);
        points[1] = b.sjeciste(c);
        points[2] = c.sjeciste(a);
    }

    vec2f CentarOpisaneKruznice()
    {
        Edge okomica1 = edges[0].okomica();
        Edge okomica2 = edges[1].okomica();

        return okomica1.sjeciste(okomica2);
    }

    float Povrsina()
    {
        float a = edges[0].length;
        float b = edges[1].length;
        float c = edges[2].length;

        float s = (a + b + c) / 2;

        return sqrtf(s * (s - a) * (s - b) * (s - c));
    }

    bool isPointInCircumcircle(vec2f point)
    {
        vec2f circumCenter = this->CentarOpisaneKruznice();

        float a = edges[0].length;
        float b = edges[1].length;
        float c = edges[2].length;

        float radius = (a * b * c) / (4 * this->Povrsina());

        return sqrtf(powf(point.x - circumCenter.x, 2) + powf(point.y - circumCenter.y, 2)) <= radius;
    }

    bool equals(Triangle othr)
    {
        for (auto& p : points)
        {
            bool samePoint = false;
            for (auto& p2 : othr.points)
            {
                if (p.x == p2.x && p.y == p2.y)
                {
                    samePoint = true;
                    break;
                }
            }

            if (!samePoint)
                return false;
        }
        return true;
    }

    vec2f points[3];
    Edge edges[3];

    bool isBad = false;
};